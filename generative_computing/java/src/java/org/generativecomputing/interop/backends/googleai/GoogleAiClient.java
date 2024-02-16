/* Copyright 2023, The Generative Computing Authors.

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
package src.java.org.generativecomputing.interop.backends.googleai;

import com.google.common.flogger.FluentLogger;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.protobuf.ByteString;
import com.google.protobuf.ExtensionRegistryLite;
import com.google.protobuf.InvalidProtocolBufferException;
import org.generativecomputing.Value;
import src.java.org.generativecomputing.interop.network.HttpClientImpl;
import src.java.org.generativecomputing.interop.network.api.proto.HttpOptions;
import src.java.org.generativecomputing.interop.network.api.proto.HttpRequest;
import src.java.org.generativecomputing.interop.network.api.proto.HttpResponse;

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

  private static final FluentLogger logger = FluentLogger.forEnclosingClass();
  private final HttpClientImpl httpClient;

  public GoogleAiClient(HttpClientImpl httpClient) {
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
          HttpRequest.QueryParam.newBuilder().setParam("key").setValue(apiKey).build());
    }
    httpRequestBuilder.setPostBody(
        HttpRequest.PostBody.newBuilder().setData(ByteString.copyFrom(request)).build());
    return httpRequestBuilder.build();
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
    Value endpoint = config.getStruct().getElement(0);
    Value authenticationToken = config.getStruct().getElement(1);
    String url = endpoint.getStr();
    if (url.isEmpty()) {
      logger.atSevere().log(
          "For model uri: %s, endpoint uri is empty in model config: %s",
          modelUri, modelConfigProto);
      return response;
    }
    String apiKey = "";
    String accessToken = "";
    if (authenticationToken.getLabel().equals("api_key")) {
      apiKey = authenticationToken.getStr();
    } else if (authenticationToken.getLabel().equals("access_token")) {
      accessToken = authenticationToken.getStr();
    }

    if (apiKey.isEmpty() && accessToken.isEmpty()) {
      logger.atSevere().log(
          "For model uri: %s, expected one of api_key or access_token to be set. Found none: %s",
          modelUri, modelConfigProto);
      return response;
    }
    HttpOptions httpOptions = createHttpOptions(url, accessToken, HttpOptions.HttpMethod.POST);
    HttpRequest httpRequest = createHttpRequestWithPostBody(apiKey, request);

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
