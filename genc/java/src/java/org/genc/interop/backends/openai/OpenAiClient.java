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
package src.java.org.genc.interop.backends.openai;

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
 * A client to connect to OpenAI chat completions API. Fills in the needed headers, authentication
 * key, and request in post body. Schedules the send over the GenC HTTP client. Processes and
 * returns response. Handles errors.
 */
public final class OpenAiClient {
  private static final String KEY_CONTENT_TYPE = "Content-Type";
  private static final String CONTENT_TYPE = "application/json";
  public static final String KEY_AUTHORIZATION = "Authorization";
  private static final String AUTHORIZATION_HEADER_PREFIX = "Bearer ";

  private static final String KEY_API_KEY = "api_key";
  private static final String KEY_ENDPOINT = "endpoint";
  private static final String KEY_JSON_REQUEST_TEMPLATE = "json_request_template";
  private static final String KEY_JSON_REQUEST_TEMPLATE_MESSAGES = "messages";
  private static final String KEY_JSON_REQUEST_TEMPLATE_CONTENT = "content";

  private static final FluentLogger logger = FluentLogger.forEnclosingClass();
  private final HttpClient httpClient;

  public OpenAiClient(HttpClient httpClient) {
    this.httpClient = httpClient;
  }

  private HttpOptions createHttpOptions(
      String url, String apiKey, HttpOptions.HttpMethod httpMethod) {
    HttpOptions.Builder httpOptionsBuilder =
        HttpOptions.newBuilder().setUrl(url).setHttpMethod(httpMethod);
    httpOptionsBuilder.addHttpHeaders(
        HttpOptions.HttpHeader.newBuilder()
            .setName(KEY_CONTENT_TYPE)
            .setValue(CONTENT_TYPE)
            .build());
    httpOptionsBuilder.addHttpHeaders(
        HttpOptions.HttpHeader.newBuilder()
            .setName(KEY_AUTHORIZATION)
            .setValue(AUTHORIZATION_HEADER_PREFIX + apiKey)
            .build());
    return httpOptionsBuilder.build();
  }

  private HttpRequest createHttpRequestWithPostBody(byte[] request) {
    HttpRequest.Builder httpRequestBuilder = HttpRequest.newBuilder();
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
        || (!configSettings.containsKey(KEY_API_KEY) || configSettings.get(KEY_API_KEY).isEmpty())
        || (!configSettings.containsKey(KEY_JSON_REQUEST_TEMPLATE)
            || configSettings.get(KEY_JSON_REQUEST_TEMPLATE).isEmpty())) {
      logger.atSevere().log(
          "For model uri: %s, required config settings were not provided. Expected 'endpoint',"
              + " 'api_key', and 'json_request_template'. Found: %s",
          modelUri, Arrays.asList(configSettings.keySet()));
      return false;
    }
    return true;
  }

  private String updateJsonRequest(String jsonRequestTemplate, String requestStr) {
    // Parse the JSON string
    JsonObject jsonObject = JsonParser.parseString(jsonRequestTemplate).getAsJsonObject();
    // Get the "messages" array, for the last occurrence of "content", update its text with request.
    JsonArray messagesArray = jsonObject.getAsJsonArray(KEY_JSON_REQUEST_TEMPLATE_MESSAGES);
    if (messagesArray.size() == 0) {
      logger.atSevere().log(
          "Invalid json request template, missing 'messages': %s", jsonRequestTemplate);
      return "";
    }
    JsonObject lastMessage = messagesArray.get(messagesArray.size() - 1).getAsJsonObject();
    lastMessage.addProperty(KEY_JSON_REQUEST_TEMPLATE_CONTENT, requestStr);
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

    // Get endpoint, api key, json request template from config
    if (modelConfigProto.getIntrinsic().getStaticParameter().getStruct().getElementCount() != 2) {
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
    String jsonRequestTemplate = configSettings.get(KEY_JSON_REQUEST_TEMPLATE);

    // Parse string out of request
    String requestStr = new String(request, UTF_8);
    String updatedRequestStr = updateJsonRequest(jsonRequestTemplate, requestStr);
    if (updatedRequestStr.isEmpty()) {
      return response;
    }
    byte[] updatedRequestBytes = updatedRequestStr.getBytes(UTF_8);

    HttpRequest httpRequest = createHttpRequestWithPostBody(updatedRequestBytes);
    HttpOptions httpOptions = createHttpOptions(endpoint, apiKey, HttpOptions.HttpMethod.POST);

    try {

      ListenableFuture<HttpResponse> future = httpClient.send(httpRequest, httpOptions);
      HttpResponse httpResponse = future.get();
      response = httpResponse.getResponse().toStringUtf8();
    } catch (Exception e) {
      if (e instanceof InterruptedException) {
        Thread.currentThread().interrupt();
      }
      logger.atSevere().withCause(e).log("OpenAI client returned error.");
    }
    return response;
  }
}
