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

import static com.google.common.util.concurrent.MoreExecutors.directExecutor;
import static java.lang.Math.min;

import android.net.Uri;
import com.google.common.flogger.FluentLogger;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.common.util.concurrent.SettableFuture;
import com.google.protobuf.ByteString;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Executor;
import org.chromium.net.CronetEngine;
import org.chromium.net.CronetException;
import org.chromium.net.UploadDataProvider;
import org.chromium.net.UploadDataSink;
import org.chromium.net.UrlRequest;
import org.chromium.net.UrlResponseInfo;
import src.java.org.genc.interop.network.api.proto.HttpOptions;
import src.java.org.genc.interop.network.api.proto.HttpRequest;
import src.java.org.genc.interop.network.api.proto.HttpResponse;

/**
 * An implementation of the HttpClient interface. Uses Cronet's Async API underneath to send the
 * requests to chosen HTTP server.
 */
public class HttpClientImpl implements HttpClient {
  private static final FluentLogger logger = FluentLogger.forEnclosingClass();
  private static final String CONTENT_TYPE_HEADER = "Content-Type";
  private final CronetEngine cronetEngine;

  /**
   * This executor uses Cronet's Java thread directly to write requests and read incremental
   * response bytes. Once the response is complete, the {@link finishingExecutor} executor is used.
   */
  private static final Executor cronetWritingAndReadingExecutor = directExecutor();

  /**
   * This executor uses the network stack thread pool. It is used to process the response once it is
   * complete or final and allows getting off of the Cronet thread {@link
   * cronetWritingAndReadingExecutor} as soon as possible to free it up for writing and reading
   * other requests/responses.
   */
  private final Executor finishingExecutor;

  public HttpClientImpl(CronetEngine cronetEngine, Executor executor) {
    if (cronetEngine == null) {
      throw new NullPointerException("Null cronetEngine provided.");
    }
    this.cronetEngine = cronetEngine;
    this.finishingExecutor = executor;
    if (finishingExecutor == null) {
      throw new NullPointerException("Null finishingExecutor provided.");
    }
  }

  private String createHttpRequestUrl(HttpRequest httpRequest, HttpOptions httpOptions) {
    Uri.Builder uriBuilder = Uri.parse(httpOptions.getUrl()).buildUpon();
    for (HttpRequest.QueryParam queryParam : httpRequest.getQueryParamsList()) {
      uriBuilder.appendQueryParameter(queryParam.getParam(), queryParam.getValue());
    }
    return uriBuilder.toString();
  }

  @Override
  public ListenableFuture<HttpResponse> send(HttpRequest request, HttpOptions httpOptions) {
    SettableFuture<HttpResponse> settableFuture = SettableFuture.create();
    UrlRequest.Callback urlRequestCallback =
        new HttpUrlRequestCallback(settableFuture, httpOptions, new HttpResponseReader());
    final String urlRequest = createHttpRequestUrl(request, httpOptions);
    final UrlRequest.Builder builder =
        cronetEngine
            .newUrlRequestBuilder(urlRequest, urlRequestCallback, cronetWritingAndReadingExecutor)
            .allowDirectExecutor();

    if (httpOptions.getHttpMethod() == HttpOptions.HttpMethod.GET) {
      builder.setHttpMethod("GET");
    } else if (httpOptions.getHttpMethod() == HttpOptions.HttpMethod.POST) {
      builder.setHttpMethod("POST");
    } else {
      // Default to GET.
      builder.setHttpMethod("GET");
    }

    // Add HTTP headers.
    for (HttpOptions.HttpHeader httpHeader : httpOptions.getHttpHeadersList()) {
      builder.addHeader(httpHeader.getName(), httpHeader.getValue());
    }

    if (request.hasPostBody()) {
      if (request.getPostBody().hasContentType()) {
        builder.addHeader(CONTENT_TYPE_HEADER, request.getPostBody().getContentType());
      }
      builder.setUploadDataProvider(
          new HttpClientUploadDataProvider(request.getPostBody().getData().toByteArray()),
          directExecutor());
    }
    final UrlRequest finalUrlRequest = builder.build();

    // If someone cancels our future, cancel the HTTP request
    settableFuture.addListener(
        () -> {
          logger.atInfo().log("HTTP request complete, cancelled=%b", settableFuture.isCancelled());
          if (settableFuture.isCancelled()) {
            finalUrlRequest.cancel();
          }
        },
        directExecutor());

    logger.atInfo().log("Sending HTTP request to: %s", urlRequest);
    finalUrlRequest.start();
    return settableFuture;
  }

  /** An implementation of Cronet's UploadDataProvider used to upload post body. */
  public static final class HttpClientUploadDataProvider extends UploadDataProvider {
    public final byte[] bytes;
    private int position = 0;

    public HttpClientUploadDataProvider(byte[] byteArray) {
      bytes = byteArray;
    }

    public byte[] getBytes() {
      return bytes;
    }

    @Override
    public long getLength() {
      return bytes.length;
    }

    @Override
    public void read(UploadDataSink uploadDataSink, ByteBuffer uploadByteBuffer) {
      int readLength = min(uploadByteBuffer.remaining(), bytes.length - position);
      uploadByteBuffer.put(bytes, position, readLength);
      position += readLength;
      uploadDataSink.onReadSucceeded(false /* Non-chunked upload, so finalChunk must be false */);
    }

    @Override
    public void rewind(UploadDataSink uploadDataSink) {
      position = 0;
      uploadDataSink.onRewindSucceeded();
    }
  }

  /** Converts raw bytes from an HTTP response to the appropriate response proto. */
  static final class HttpResponseReader {
    HttpResponse readResponseBody(ByteBuffer responseBuffer) {
      byte[] bytes = new byte[responseBuffer.remaining()];
      responseBuffer.get(bytes);
      HttpResponse responseProto =
          HttpResponse.newBuilder().setResponse(ByteString.copyFrom(bytes)).build();
      return responseProto;
    }
  }

  // A container to hold byte buffers received from the Cronet read calls. These byte buffers
  // collectively make the full response body for a single response.
  private static final class ByteBufferList {
    final List<ByteBuffer> bufferList = new ArrayList<>();

    void add(ByteBuffer buffer) {
      bufferList.add(buffer);
    }

    ByteBuffer coalesce() {
      if (bufferList.isEmpty()) {
        return ByteBuffer.allocateDirect(0);
      }
      // If there is only a single byte buffer, then return it without copying into a separate
      // buffer.
      if (bufferList.size() == 1) {
        ByteBuffer result = bufferList.get(0);
        if (result.hasRemaining()) {
          // Limit the size of the buffer to the current position and set the position to zero.
          result.flip();
        }
        result.position(0); // reposition to 0 in case flip didn't happen on a full buffer.
        return result;
      }
      // Copy all the byte buffers into a single larger buffer now that we know the actual size.
      int size = 0;
      for (ByteBuffer buffer : bufferList) {
        buffer.flip();
        size += buffer.remaining();
      }
      ByteBuffer resultBuffer = ByteBuffer.allocateDirect(size);
      for (ByteBuffer buffer : bufferList) {
        resultBuffer.put(buffer);
      }
      resultBuffer.flip();
      return resultBuffer;
    }
  }

  /**
   * An implementation of Cronet's {@link org.chromium.net.UrlRequest.Callback} which overrides key
   * async methods of Cronet's async API inorder to read the response or errors encountered, and
   * sets the input SettableFuture with a valid response proto or an exception which will get thrown
   * when the future's get() method is called.
   */
  public final class HttpUrlRequestCallback extends UrlRequest.Callback {
    public static final int BYTE_BUFFER_LENGTH = 128 * 1024; // 128KB
    private static final String CONTENT_LENGTH = "Content-Length";
    private static final int HTTP_STATUS_OK = 200;
    private final SettableFuture<HttpResponse> settableFuture;
    private final HttpResponseReader responseReader;
    private final ByteBufferList bufferList = new ByteBufferList();
    private final HttpOptions httpOptions;
    private int redirectRemaining = 0;

    public HttpUrlRequestCallback(
        SettableFuture<HttpResponse> settableFuture,
        HttpOptions httpOptions,
        HttpResponseReader responseReader) {
      this.settableFuture = settableFuture;
      this.responseReader = responseReader;
      this.httpOptions = httpOptions;
      this.redirectRemaining = httpOptions.getRedirectsToFollow();
    }

    /**
     * Returns an estimated buffer size that can hold the whole response. This based on the content
     * length header in the response. Returns BYTE_BUFFER_LENGTH if content length is not present in
     * the response.
     */
    private int getResponseBufferSizeEstimate(UrlResponseInfo info) {
      Map<String, List<String>> headers = info.getAllHeaders();
      if (headers.containsKey(CONTENT_LENGTH)) {
        // +1 as this helps in avoiding allocation of a new byte buffer in Cronet's onReadResponse
        // function as the response is fully read in the byteBuffer and there is still some extra
        // space available. Cronet will subsequently call the onSucceeded function.
        return Integer.parseInt(headers.get(CONTENT_LENGTH).get(0)) + 1;
      }
      return BYTE_BUFFER_LENGTH;
    }

    @Override
    public void onResponseStarted(UrlRequest urlRequest, UrlResponseInfo responseInfo) {
      try {
        int httpStatusCode = responseInfo.getHttpStatusCode();
        if (httpStatusCode == HTTP_STATUS_OK) {
          // Start reading the response only if the status code is OK.
          final ByteBuffer byteBuffer =
              ByteBuffer.allocateDirect(getResponseBufferSizeEstimate(responseInfo));
          bufferList.add(byteBuffer); // Add new buffer to buffer list.
          urlRequest.read(byteBuffer);
        } else {
          logger.atInfo().log("Received non StatusOK status code: %d", httpStatusCode);
          settableFuture.setException(
              new HttpException("Http request failed with status code: " + httpStatusCode));
        }
      } catch (RuntimeException e) {
        settableFuture.setException(e);
      }
    }

    @Override
    public void onReadCompleted(
        UrlRequest urlRequest, UrlResponseInfo urlResponse, ByteBuffer byteBuffer) {
      try {
        if (byteBuffer.hasRemaining()) {
          urlRequest.read(byteBuffer); // Cronet can read more bytes into the current buffer.
        } else {
          // Buffer is full, allocate a new byte buffer. Also, add the new buffer to the bufferList
          // as we may not get another call to onReadCompleted if this buffer is sufficient to read
          // the remaining response.
          final ByteBuffer buffer = ByteBuffer.allocateDirect(BYTE_BUFFER_LENGTH);
          bufferList.add(buffer);
          urlRequest.read(buffer);
        }
      } catch (RuntimeException e) {
        settableFuture.setException(e);
      }
    }

    @Override
    public void onSucceeded(UrlRequest urlRequest, UrlResponseInfo urlResponse) {
      finishingExecutor.execute(
          () -> {
            try {
              // Coalesce the bufferList into one buffer and send it to responseReader to read the
              // response.
              ByteBuffer responseBody = bufferList.coalesce();
              HttpResponse response = responseReader.readResponseBody(responseBody);
              settableFuture.set(response);
            } catch (RuntimeException e) {
              logger.atInfo().log("Failed to read the response!");
              settableFuture.setException(e);
            }
          });
    }

    @Override
    public void onFailed(
        UrlRequest urlRequest, UrlResponseInfo urlResponse, CronetException cronetException) {
      finishingExecutor.execute(
          () -> {
            settableFuture.setException(new HttpException("Http request failed", cronetException));
          });
    }

    @Override
    public void onCanceled(UrlRequest urlRequest, UrlResponseInfo urlResponse) {
      finishingExecutor.execute(
          () -> {
            settableFuture.cancel(false);
          });
    }

    @Override
    public final void onRedirectReceived(
        UrlRequest request, UrlResponseInfo responseInfo, String newLocationUrl) {
      if (redirectRemaining > 0) {
        redirectRemaining--;
        request.followRedirect();
      } else {
        request.cancel();
        settableFuture.setException(
            new HttpException(
                "More redirects than allowed: " + httpOptions.getRedirectsToFollow()));
      }
    }
  }
}
