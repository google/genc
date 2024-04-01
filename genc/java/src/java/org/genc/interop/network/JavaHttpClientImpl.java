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

package src.java.org.genc.interop.network;

import static java.nio.charset.StandardCharsets.UTF_8;

import com.google.common.flogger.FluentLogger;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.common.util.concurrent.SettableFuture;
import com.google.protobuf.ByteString;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URLEncoder;
import java.net.http.HttpRequest.BodyPublishers;
import java.net.http.HttpResponse.BodyHandlers;
import java.util.concurrent.Executor;
import src.java.org.genc.interop.network.api.proto.HttpOptions;
import src.java.org.genc.interop.network.api.proto.HttpRequest;
import src.java.org.genc.interop.network.api.proto.HttpResponse;

/** An implementation of the HttpClient interface. */
public class JavaHttpClientImpl implements HttpClient {
  private static final int HTTP_STATUS_OK = 200;
  private static final FluentLogger logger = FluentLogger.forEnclosingClass();

  /**
   * This executor uses the network stack thread pool. It is used to process the response once it is
   * complete or final.
   */
  private final Executor executor;

  public JavaHttpClientImpl(Executor executor) {
    if (executor == null) {
      throw new NullPointerException("Null executor provided.");
    }
    this.executor = executor;
  }

  private String createHttpRequestUrl(HttpRequest httpRequest, HttpOptions httpOptions)
      throws URISyntaxException {
    URI uri = new URI(httpOptions.getUrl());
    // Add query parameters
    String queryString = uri.getQuery();
    for (HttpRequest.QueryParam queryParam : httpRequest.getQueryParamsList()) {
      String key = URLEncoder.encode(queryParam.getParam(), UTF_8);
      String value = URLEncoder.encode(queryParam.getValue(), UTF_8);
      if (queryString == null) {
        queryString = key + "=" + value;
      } else {
        queryString = queryString + "&" + key + "=" + value;
      }
    }

    // Create updated URI with the new query parameters
    URI updatedUri =
        new URI(uri.getScheme(), uri.getAuthority(), uri.getPath(), queryString, uri.getFragment());
    return updatedUri.toString();
  }

  @Override
  public ListenableFuture<HttpResponse> send(HttpRequest request, HttpOptions httpOptions) {
    SettableFuture<HttpResponse> settableFuture = SettableFuture.create();
    try {
      java.net.http.HttpClient httpClient = java.net.http.HttpClient.newHttpClient();
      HttpResponseReader responseReader = new HttpResponseReader();
      // Create Url with query params.
      final String urlRequest = createHttpRequestUrl(request, httpOptions);
      final java.net.http.HttpRequest.Builder builder =
          java.net.http.HttpRequest.newBuilder().uri(URI.create(urlRequest));

      // Add HTTP headers.
      for (HttpOptions.HttpHeader httpHeader : httpOptions.getHttpHeadersList()) {
        builder.header(httpHeader.getName(), httpHeader.getValue());
      }

      // Set Method and add post body if present.
      if (httpOptions.getHttpMethod() == HttpOptions.HttpMethod.GET) {
        builder.GET();
      } else if (httpOptions.getHttpMethod() == HttpOptions.HttpMethod.POST) {
        builder.POST(BodyPublishers.ofByteArray(request.getPostBody().getData().toByteArray()));
      } else {
        // Default to GET.
        builder.GET();
      }

      final java.net.http.HttpRequest finalHttpRequest = builder.build();

      executor.execute(
          () -> {
            try {
              logger.atFine().log("Sending HTTP request to: %s", finalHttpRequest.uri());
              java.net.http.HttpResponse<byte[]> httpResponse =
                  httpClient.send(finalHttpRequest, BodyHandlers.ofByteArray());
              if (httpResponse.statusCode() == HTTP_STATUS_OK) {
                src.java.org.genc.interop.network.api.proto.HttpResponse response =
                    responseReader.readResponseBody(httpResponse.body());
                settableFuture.set(response);
              } else {
                logger.atSevere().log(
                    "Received non StatusOK status code: %d", httpResponse.statusCode());
                settableFuture.setException(
                    new HttpException(
                        "Http request failed with status code: " + httpResponse.statusCode()));
              }
            } catch (Exception e) {
              if (e instanceof InterruptedException) {
                Thread.currentThread().interrupt();
              }
              logger.atInfo().log("Failure encountered in sending HTTP request!");
              settableFuture.setException(e);
            }
          });
    } catch (Exception e) {
      settableFuture.setException(e);
    }
    return settableFuture;
  }

  /** Converts response body from an HTTP response to response proto. */
  static final class HttpResponseReader {
    HttpResponse readResponseBody(byte[] responseBody) {
      return HttpResponse.newBuilder().setResponse(ByteString.copyFrom(responseBody)).build();
    }
  }
}
