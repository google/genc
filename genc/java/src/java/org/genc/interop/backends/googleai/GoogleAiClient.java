/* Copyright 2023, The GenC Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License
==============================================================================*/
package src.java.org.genc.interop.backends.googleai;

import static java.nio.charset.StandardCharsets.UTF_8;

import com.google.common.flogger.FluentLogger;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.google.protobuf.ByteString;
import com.google.protobuf.ExtensionRegistryLite;
import com.google.protobuf.InvalidProtocolBufferException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import org.genc.Value;
import src.java.org.genc.interop.network.HttpClient;
import src.java.org.genc.interop.network.api.proto.HttpOptions;
import src.java.org.genc.interop.network.api.proto.HttpRequest;
import src.java.org.genc.interop.network.api.proto.HttpResponse;

/**
 * A client to connect to Google AI Gemini APIs. Fills in the needed headers, authentication key,
 * and request in post body. Schedules the send over the HTTP client through Cronet. Processes and
 * returns response. Handles errors.
 */
public final class GoogleAiClient {
  private static final String KEY_CONTENT_TYPE = "Content-Type";
  private static final String CONTENT_TYPE = "application/json";
  public static final String KEY_AUTHORIZATION = "Authorization";
  private static final String AUTHORIZATION_HEADER_PREFIX = "Bearer ";
  private static final String KEY_QUERY_PARAM_API_KEY = "key";

  private static final String KEY_API_KEY = "api_key";
  private static final String KEY_ACCESS_TOKEN = "access_token";
  private static final String KEY_ENDPOINT = "endpoint";
  private static final String KEY_JSON_REQUEST_TEMPLATE = "json_request_template";
  private static final String KEY_JSON_REQUEST_TEMPLATE_CONTENTS = "contents";
  private static final String KEY_JSON_REQUEST_TEMPLATE_TEXT = "text";
  private static final String KEY_JSON_REQUEST_TEMPLATE_PARTS = "parts";

  private static final FluentLogger logger = FluentLogger.forEnclosingClass();
  private final HttpClient httpClient;

  public GoogleAiClient(HttpClient httpClient) {
    this.httpClient = httpClient;
  }

  private HttpOptions createHttpOptions(
      String url, String accessToken, HttpOptions.HttpMethod httpMethod) {
    if (url == null || url.isEmpty()) {
      throw new IllegalArgumentException("url is null or empty");
    }
    HttpOptions.Builder httpOptionsBuilder =
        HttpOptions.newBuilder().setUrl(url).setHttpMethod(httpMethod);
    httpOptionsBuilder.addHttpHeaders(
        HttpOptions.HttpHeader.newBuilder()
            .setName(KEY_CONTENT_TYPE)
            .setValue(CONTENT_TYPE)
            .build());
    if (accessToken != null && !accessToken.isEmpty()) {
      httpOptionsBuilder.addHttpHeaders(
          HttpOptions.HttpHeader.newBuilder()
              .setName(KEY_AUTHORIZATION)
              .setValue(AUTHORIZATION_HEADER_PREFIX + accessToken)
              .build());
    }
    return httpOptionsBuilder.build();
  }

  private HttpRequest createHttpRequestWithPostBody(String apiKey, byte[] request) {
    HttpRequest.Builder httpRequestBuilder = HttpRequest.newBuilder();
    if (apiKey != null && !apiKey.isEmpty()) {
      httpRequestBuilder.addQueryParams(
          HttpRequest.QueryParam.newBuilder()
              .setParam(KEY_QUERY_PARAM_API_KEY)
              .setValue(apiKey)
              .build());
    }
    httpRequestBuilder.setPostBody(
        HttpRequest.PostBody.newBuilder().setData(ByteString.copyFrom(request)).build());
    return httpRequestBuilder.build();
  }

  private static Map<String, String> getConfigSettings(Value config) {
    Map<String, String> configSettings = new HashMap<>();
    for (int i = 0; i < config.getStruct().getElementCount(); i++) {
      configSettings.put(
          config.getStruct().getElement(i).getLabel(), config.getStruct().getElement(i).getStr());
    }
    return configSettings;
  }

  private boolean isValidConfigSettings(Map<String, String> configSettings, String modelUri) {
    if ((!configSettings.containsKey(KEY_ENDPOINT) || configSettings.get(KEY_ENDPOINT).isEmpty())
        || (!configSettings.containsKey(KEY_JSON_REQUEST_TEMPLATE)
            || configSettings.get(KEY_JSON_REQUEST_TEMPLATE).isEmpty())
        || (!configSettings.containsKey(KEY_API_KEY)
            && !configSettings.containsKey(KEY_ACCESS_TOKEN))
        || (configSettings.containsKey(KEY_API_KEY) && configSettings.get(KEY_API_KEY).isEmpty())
        || (configSettings.containsKey(KEY_ACCESS_TOKEN)
            && configSettings.get(KEY_ACCESS_TOKEN).isEmpty())) {
      logger.atSevere().log(
          "For model uri: %s, required config settings were not provided. Expected 'endpoint',"
              + " 'json_request_template', and one of 'api_key' or 'access_token'. Found: %s",
          modelUri, Arrays.asList(configSettings));
      return false;
    }
    return true;
  }

  private String updateJsonRequest(String jsonRequestTemplate, String requestStr) {
    // Parse the JSON string
    JsonObject jsonObject = JsonParser.parseString(jsonRequestTemplate).getAsJsonObject();

    // Get the "contents" array
    JsonArray contentsArray = jsonObject.getAsJsonArray(KEY_JSON_REQUEST_TEMPLATE_CONTENTS);
    if (contentsArray.size() == 0) {
      logger.atSevere().log(
          "Invalid json request template, missing 'contents': %s", jsonRequestTemplate);
      return "";
    }
    // Find the last occurrence of "contents" and update its last occurence of "parts" with "text"
    // object
    JsonObject item = contentsArray.get(contentsArray.size() - 1).getAsJsonObject();
    JsonArray partsArray = item.getAsJsonArray(KEY_JSON_REQUEST_TEMPLATE_PARTS);
    if (partsArray == null || partsArray.size() == 0) {
      logger.atSevere().log(
          "Invalid json request template, missing 'parts': %s", jsonRequestTemplate);
      return "";
    }
    JsonObject lastPartObject = partsArray.get(partsArray.size() - 1).getAsJsonObject();
    lastPartObject.addProperty(KEY_JSON_REQUEST_TEMPLATE_TEXT, requestStr);

    // Convert the modified JsonObject back to a JSON string
    return jsonObject.toString();
  }

  // Returns best available ExtensionRegistry.
  public static ExtensionRegistryLite getExtensionRegistry() {
    return ExtensionRegistryLite.getEmptyRegistry();
  }

  public String call(byte[] modelInferenceWithConfig, byte[] request) {
    String response = "";
    Value modelConfigProto;
    try {
      modelConfigProto = Value.parseFrom(modelInferenceWithConfig, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      logger.atSevere().withCause(e).log("Could not parse Value proto: %s", e.getMessage());
      return response;
    }

    // Get url, api key or access token from config
    if (modelConfigProto.getIntrinsic().getStaticParameter().getStruct().getElementCount() < 2) {
      logger.atSevere().log(
          "Model config missing necessary fields. Number of fields provided: %d. Number expected: 2"
              + " (model_uri, model_config)",
          modelConfigProto.getIntrinsic().getStaticParameter().getStruct().getElementCount());
    }
    Value modelUri = modelConfigProto.getIntrinsic().getStaticParameter().getStruct().getElement(0);
    Value config = modelConfigProto.getIntrinsic().getStaticParameter().getStruct().getElement(1);
    Map<String, String> configSettings = getConfigSettings(config);

    if (!isValidConfigSettings(configSettings, modelUri.getStr())) {
      return response;
    }

    String endpoint = configSettings.get(KEY_ENDPOINT);
    String apiKey = configSettings.get(KEY_API_KEY);
    String accessToken = configSettings.get(KEY_ACCESS_TOKEN);
    String jsonRequestTemplate = configSettings.get(KEY_JSON_REQUEST_TEMPLATE);

    // Parse string out of request
    String requestStr = new String(request, UTF_8);
    String updatedRequestStr = updateJsonRequest(jsonRequestTemplate, requestStr);
    byte[] updatedRequestBytes = updatedRequestStr.getBytes(UTF_8);
    if (updatedRequestStr.isEmpty()) {
      return response;
    }

    HttpOptions httpOptions = createHttpOptions(endpoint, accessToken, HttpOptions.HttpMethod.POST);
    HttpRequest httpRequest = createHttpRequestWithPostBody(apiKey, updatedRequestBytes);

    try {
      logger.atInfo().log(
          "Sending request to Google AI: HttpRequest: %s, HttpOptions: %s",
          httpRequest, httpOptions);
      ListenableFuture<HttpResponse> future = httpClient.send(httpRequest, httpOptions);
      HttpResponse httpResponse = future.get();
      logger.atInfo().log("Received response from Google AI: %s", httpResponse);
      response = httpResponse.getResponse().toStringUtf8();
    } catch (Exception e) {
      if (e instanceof InterruptedException) {
        Thread.currentThread().interrupt();
      }
      logger.atWarning().withCause(e).log("Google AI client returned error: %s", e.getMessage());
    }
    return response;
  }
}
