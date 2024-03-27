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

package src.java.org.generativecomputing.interop.backends.wolframalpha;

import static java.nio.charset.StandardCharsets.UTF_8;

import com.google.common.flogger.FluentLogger;
import com.google.common.util.concurrent.ListenableFuture;
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
public final class WolframAlphaClient {
  private static final String ENDPOINT_URL = "https://api.wolframalpha.com/v2/query";
  private static final String KEY_APPID = "appid";
  private static final String KEY_INPUT = "input";
  private static final String KEY_OUTPUT = "output";
  private static final String OUTPUT_VALUE = "json";
  private static final String KEY_INCLUDE_POD_ID = "includepodid";
  private static final String INCLUDE_POD_ID_VALUE = "Result";

  private static final FluentLogger logger = FluentLogger.forEnclosingClass();
  private final HttpClientImpl httpClient;

  public WolframAlphaClient(HttpClientImpl httpClient) {
    this.httpClient = httpClient;
  }

  private HttpOptions createHttpOptions(String url, HttpOptions.HttpMethod httpMethod) {
    if (url == null || url.isEmpty()) {
      throw new IllegalArgumentException("url is null or empty");
    }
    HttpOptions httpOptions =
        HttpOptions.newBuilder().setUrl(url).setHttpMethod(httpMethod).build();
    return httpOptions;
  }

  private HttpRequest createHttpRequest(String appId, String request) {
    if (appId == null || appId.isEmpty()) {
      throw new IllegalArgumentException("app id is null or empty");
    }
    if (request == null || request.isEmpty()) {
      throw new IllegalArgumentException("request is null or empty");
    }
    HttpRequest.Builder httpRequestBuilder = HttpRequest.newBuilder();
    httpRequestBuilder.addQueryParams(
        HttpRequest.QueryParam.newBuilder().setParam(KEY_APPID).setValue(appId).build());
    httpRequestBuilder.addQueryParams(
        HttpRequest.QueryParam.newBuilder().setParam(KEY_OUTPUT).setValue(OUTPUT_VALUE).build());
    httpRequestBuilder.addQueryParams(
        HttpRequest.QueryParam.newBuilder()
            .setParam(KEY_INCLUDE_POD_ID)
            .setValue(INCLUDE_POD_ID_VALUE)
            .build());
    httpRequestBuilder.addQueryParams(
        HttpRequest.QueryParam.newBuilder().setParam(KEY_INPUT).setValue(request).build());
    return httpRequestBuilder.build();
  }

  // Returns best available ExtensionRegistry.
  public static ExtensionRegistryLite getExtensionRegistry() {
    return ExtensionRegistryLite.getEmptyRegistry();
  }

  public String call(byte[] config, byte[] request) {
    Value configProto;
    String response = "";
    try {
      configProto = Value.parseFrom(config, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      logger.atSevere().withCause(e).log("Could not parse Value proto: %s", e.getMessage());
      return response;
    }

    // Get app id from config
    String appIdStr = configProto.getIntrinsic().getStaticParameter().getStr();
    String requestStr = new String(request, UTF_8);
    try {
      HttpOptions httpOptions = createHttpOptions(ENDPOINT_URL, HttpOptions.HttpMethod.GET);
      HttpRequest httpRequest = createHttpRequest(appIdStr, requestStr);
      logger.atInfo().log(
          "Sending request to Wolfram Alpha: HttpRequest: %s, HttpOptions: %s",
          httpRequest, httpOptions);
      ListenableFuture<HttpResponse> future = httpClient.send(httpRequest, httpOptions);
      HttpResponse httpResponse = future.get();
      logger.atInfo().log("Received response from Wolfram Alpha: %s", httpResponse);
      response = httpResponse.getResponse().toStringUtf8();
    } catch (Exception e) {
      if (e instanceof InterruptedException) {
        Thread.currentThread().interrupt();
      }
      logger.atWarning().withCause(e).log(
          "Wolfram Alpha client returned error: %s", e.getMessage());
    }
    return response;
  }
}
