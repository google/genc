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

import com.google.common.util.concurrent.ListenableFuture;
import src.java.org.genc.interop.network.api.proto.HttpOptions;
import src.java.org.genc.interop.network.api.proto.HttpRequest;
import src.java.org.genc.interop.network.api.proto.HttpResponse;

/** A simple synchronous HTTP client interface that matches a subset of the API defined in C++. */
public final class SimpleSynchronousHttpClientInterface {
  private final HttpClient httpClient;

  public SimpleSynchronousHttpClientInterface(HttpClient httpClient) {
    this.httpClient = httpClient;
  }

  public String getFromUrl(byte[] url) {
    String urlString = new String(url);
    HttpRequest httpRequest = HttpRequest.getDefaultInstance();
    HttpOptions httpOptions =
        HttpOptions.newBuilder()
            .setUrl(urlString)
            .setHttpMethod(HttpOptions.HttpMethod.GET)
            .build();
    String response = "";
    try {
      ListenableFuture<HttpResponse> future = httpClient.send(httpRequest, httpOptions);
      HttpResponse httpResponse = future.get();
      response = httpResponse.getResponse().toStringUtf8();
    } catch (Exception e) {
      if (e instanceof InterruptedException) {
        Thread.currentThread().interrupt();
      }
    }
    return response;
  }
}
