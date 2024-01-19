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
package src.java.org.generativecomputing.interop.backends.openai;

import com.google.common.flogger.FluentLogger;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.protobuf.ByteString;
import src.java.org.generativecomputing.interop.network.HttpClientImpl;
import src.java.org.generativecomputing.interop.network.api.proto.HttpOptions;
import src.java.org.generativecomputing.interop.network.api.proto.HttpRequest;
import src.java.org.generativecomputing.interop.network.api.proto.HttpResponse;

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

  private static final FluentLogger logger = FluentLogger.forEnclosingClass();
  private final String url;
  private final String apiKey;
  private final HttpClientImpl httpClient;

  public OpenAiClient(HttpClientImpl httpClient, String url, String apiKey) {
    this.httpClient = httpClient;
    this.url = url;
    this.apiKey = apiKey;
  }

  private HttpOptions createHttpOptions(HttpOptions.HttpMethod httpMethod) {
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

  public String call(byte[] request) {
    HttpRequest httpRequest = createHttpRequestWithPostBody(request);
    HttpOptions httpOptions = createHttpOptions(HttpOptions.HttpMethod.POST);
    String response = "";
    try {
      logger.atInfo().log(
          "Sending request to OpenAI: HttpRequest: %s, HttpOptions: %s", httpRequest, httpOptions);
      ListenableFuture<HttpResponse> future = httpClient.send(httpRequest, httpOptions);
      HttpResponse httpResponse = future.get();
      logger.atInfo().log("Received response from OpenAI: %s", httpResponse);
      response = httpResponse.getResponse().toStringUtf8();
    } catch (Exception e) {
      if (e instanceof InterruptedException) {
        Thread.currentThread().interrupt();
      }
      logger.atWarning().withCause(e).log("OpenAI client returned error: %s", e.getMessage());
    }
    return response;
  }
}
