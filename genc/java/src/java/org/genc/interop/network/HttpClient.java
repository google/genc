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

/** An interface for an asynchronous HTTP client. */
public interface HttpClient {
  /**
   * Sends a single http request asynchronously. Returns a {@link ListenableFuture} which provides
   * the response when available.
   */
  ListenableFuture<HttpResponse> send(HttpRequest request, HttpOptions httpOptions);
}
