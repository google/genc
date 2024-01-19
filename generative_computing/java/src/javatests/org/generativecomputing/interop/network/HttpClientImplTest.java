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

package src.javatests.org.generativecomputing.interop.network;

import static com.google.common.truth.Truth.assertThat;
import static java.nio.charset.StandardCharsets.UTF_8;
import static org.junit.Assert.fail;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.reset;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import com.google.common.primitives.Bytes;
import com.google.common.util.concurrent.MoreExecutors;
import com.google.protobuf.ByteString;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import org.chromium.net.CronetEngine;
import org.chromium.net.UploadDataSink;
import org.chromium.net.UrlRequest;
import org.chromium.net.UrlResponseInfo;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.ArgumentCaptor;
import org.mockito.ArgumentMatchers;
import org.mockito.Mock;
import org.mockito.junit.MockitoJUnit;
import org.mockito.junit.MockitoRule;
import src.java.org.generativecomputing.interop.network.HttpClientImpl;
import src.java.org.generativecomputing.interop.network.HttpException;
import src.java.org.generativecomputing.interop.network.api.proto.HttpOptions;
import src.java.org.generativecomputing.interop.network.api.proto.HttpRequest;
import src.java.org.generativecomputing.interop.network.api.proto.HttpResponse;

/** Tests for HttpClientImpl */
@RunWith(AndroidJUnit4.class)
public final class HttpClientImplTest {
  @Rule public final MockitoRule mocks = MockitoJUnit.rule();

  @Mock private CronetEngine mockCronetEngine;
  @Mock private UrlRequest mockUrlRequest;
  @Mock private UrlRequest.Builder mockUrlRequestBuilder;
  @Mock private UrlResponseInfo mockUrlResponseInfo;
  @Mock private UploadDataSink mockDataSink;

  private HttpClientImpl rpc;

  private static final String TEST_URL = "http://www.googleapis.com";
  private static final String CONTENT_TYPE_HEADER = "Content-Type";

  public static final String KEY_AUTHORIZATION = "Authorization";
  private static final String AUTHORIZATION_HEADER_PREFIX = "Bearer ";
  private static final String KEY_API_TOKEN = "apiToken";
  private static final String EXPECTED_GET_TEST_URL = TEST_URL + "?key1=val1&key2=val2";

  @Before
  public void setUp() {
    rpc =
        new HttpClientImpl(
            mockCronetEngine,
            MoreExecutors.listeningDecorator(Executors.newScheduledThreadPool(1)));
  }

  private HttpOptions createHttpOptions(HttpOptions.HttpMethod httpMethod) {
    HttpOptions.Builder httpOptionsBuilder =
        HttpOptions.newBuilder().setUrl(TEST_URL).setHttpMethod(httpMethod);
    httpOptionsBuilder.addHttpHeaders(
        HttpOptions.HttpHeader.newBuilder()
            .setName(KEY_AUTHORIZATION)
            .setValue(AUTHORIZATION_HEADER_PREFIX + "test-auth-code")
            .build());
    httpOptionsBuilder.addHttpHeaders(
        HttpOptions.HttpHeader.newBuilder()
            .setName(KEY_API_TOKEN)
            .setValue("test-api-token")
            .build());
    return httpOptionsBuilder.build();
  }

  private HttpRequest createHttpRequestWithQueryParams() {
    HttpRequest.Builder httpRequestBuilder = HttpRequest.newBuilder();
    httpRequestBuilder.addQueryParams(
        HttpRequest.QueryParam.newBuilder().setParam("key1").setValue("val1").build());
    httpRequestBuilder.addQueryParams(
        HttpRequest.QueryParam.newBuilder().setParam("key2").setValue("val2").build());
    return httpRequestBuilder.build();
  }

  private HttpRequest createHttpRequestWithPostBody() {
    HttpRequest.Builder httpRequestBuilder = HttpRequest.newBuilder();
    httpRequestBuilder.setPostBody(
        HttpRequest.PostBody.newBuilder()
            .setContentType("test-content-type")
            .setData(ByteString.copyFromUtf8("test-data"))
            .build());
    return httpRequestBuilder.build();
  }

  private void setupCronetEngineUrlRequestBuilderMocks() {
    when(mockCronetEngine.newUrlRequestBuilder(anyString(), any(), any()))
        .thenReturn(mockUrlRequestBuilder);
    when(mockUrlRequestBuilder.allowDirectExecutor()).thenReturn(mockUrlRequestBuilder);
    when(mockUrlRequestBuilder.setHttpMethod(anyString())).thenReturn(mockUrlRequestBuilder);
    when(mockUrlRequestBuilder.build()).thenReturn(mockUrlRequest);
  }

  private void verifyCreateRequestAndSend(
      String expectedUrl,
      String expectedHttpMethod,
      ArgumentCaptor<UrlRequest.Callback> callbackCaptor) {
    // Verify the request url is correct.
    ArgumentCaptor<String> urlCaptor = ArgumentCaptor.forClass(String.class);
    verify(mockCronetEngine)
        .newUrlRequestBuilder(
            urlCaptor.capture(), callbackCaptor.capture(), ArgumentMatchers.any(Executor.class));
    assertThat(urlCaptor.getValue()).isEqualTo(expectedUrl);

    // Verify the required HTTP headers are added.
    verify(mockUrlRequestBuilder).addHeader(KEY_AUTHORIZATION, "Bearer test-auth-code");
    verify(mockUrlRequestBuilder).addHeader(KEY_API_TOKEN, "test-api-token");
    verify(mockUrlRequestBuilder).setHttpMethod(expectedHttpMethod);

    // Verify that the url request is actually sent.
    verify(mockUrlRequest).start();
  }

  private void addHttpStatusAndContentLength(int status, int contentLength) {
    when(mockUrlResponseInfo.getHttpStatusCode()).thenReturn(status);
    Map<String, List<String>> headers = new HashMap<>();
    // Add content length header only if contentLength is > 0.
    if (contentLength > 0) {
      List<String> headerValue = new ArrayList<>();
      headerValue.add(Integer.toString(contentLength));
      headers.put("Content-Length", headerValue);
    }
    when(mockUrlResponseInfo.getAllHeaders()).thenReturn(headers);
  }

  @Test
  public void testHttpClient_sendWithGet() throws Exception {
    HttpRequest httpRequest = createHttpRequestWithQueryParams();
    HttpOptions httpOptions = createHttpOptions(HttpOptions.HttpMethod.GET);

    setupCronetEngineUrlRequestBuilderMocks();
    @SuppressWarnings("unused") // go/futurereturn-lsc
    Future<HttpResponse> future = rpc.send(httpRequest, httpOptions);

    ArgumentCaptor<UrlRequest.Callback> callbackCaptor =
        ArgumentCaptor.forClass(UrlRequest.Callback.class);
    verifyCreateRequestAndSend(EXPECTED_GET_TEST_URL, "GET", callbackCaptor);
  }

  @Test
  public void testHttpClient_sendWithPost() throws Exception {
    HttpRequest httpRequest = createHttpRequestWithPostBody();
    HttpOptions httpOptions = createHttpOptions(HttpOptions.HttpMethod.POST);

    setupCronetEngineUrlRequestBuilderMocks();
    @SuppressWarnings("unused") // go/futurereturn-lsc
    Future<HttpResponse> future = rpc.send(httpRequest, httpOptions);

    // Verify content type header and upload data provider for post are set correctly
    verify(mockUrlRequestBuilder).addHeader(CONTENT_TYPE_HEADER, "test-content-type");
    ArgumentCaptor<HttpClientImpl.HttpClientUploadDataProvider> uploadDataProviderCaptor =
        ArgumentCaptor.forClass(HttpClientImpl.HttpClientUploadDataProvider.class);
    verify(mockUrlRequestBuilder).setUploadDataProvider(uploadDataProviderCaptor.capture(), any());

    // Verify url, http headers are set correctly
    ArgumentCaptor<UrlRequest.Callback> callbackCaptor =
        ArgumentCaptor.forClass(UrlRequest.Callback.class);
    verifyCreateRequestAndSend(TEST_URL, "POST", callbackCaptor);

    // Verify post body contents
    HttpClientImpl.HttpClientUploadDataProvider uploadDataProvider =
        uploadDataProviderCaptor.getValue();
    assertThat(uploadDataProvider.getBytes()).isEqualTo("test-data".getBytes(UTF_8));
  }

  @Test
  public void testRequestUploader_read() throws IOException {
    byte[] data = "test-data".getBytes(UTF_8);
    HttpClientImpl.HttpClientUploadDataProvider uploader =
        new HttpClientImpl.HttpClientUploadDataProvider(data);
    ByteBuffer byteBuffer = ByteBuffer.allocate(512);
    uploader.read(mockDataSink, byteBuffer);
    byte[] array = new byte[byteBuffer.position()];
    // Need to reset the position.
    byteBuffer.position(0);
    byteBuffer.get(array);
    assertThat(data).isEqualTo(array);
    verify(mockDataSink).onReadSucceeded(false);
  }

  @Test
  public void testRequestUploader_read_smallDestinationBufferRequiresMultipleRead()
      throws IOException {
    byte[] data = "large-sized-test-data-which-needs-to-be-read-in-chunks".getBytes(UTF_8);
    HttpClientImpl.HttpClientUploadDataProvider uploader =
        new HttpClientImpl.HttpClientUploadDataProvider(data);
    // Create a byte buffer with size bigger than half of the data length so we can finish reading
    // the content by two reads.
    ByteBuffer byteBuffer = ByteBuffer.allocate(data.length / 2 + 5);
    // read the first 60 bytes.
    uploader.read(mockDataSink, byteBuffer);
    byte[] array = new byte[byteBuffer.position()];
    byteBuffer.position(0);
    byteBuffer.get(array);
    byteBuffer.rewind();
    // Read the rest of the bytes.
    uploader.read(mockDataSink, byteBuffer);
    byte[] array2 = new byte[byteBuffer.position()];
    byteBuffer.position(0);
    byteBuffer.get(array2);

    // combine the two arrays and compare the data.
    byte[] combinedArray = Bytes.concat(array, array2);
    assertThat(combinedArray).isEqualTo(data);
    verify(mockDataSink, times(2)).onReadSucceeded(false);
  }

  @Test
  public void testRequestUploader_getLength() throws IOException {
    byte[] data = "test-data".getBytes(UTF_8);
    HttpClientImpl.HttpClientUploadDataProvider uploader =
        new HttpClientImpl.HttpClientUploadDataProvider(data);
    assertThat(uploader.getLength()).isEqualTo(data.length);
  }

  @Test
  public void testRequestUploader_rewind() throws IOException {
    byte[] data = "test-data".getBytes(UTF_8);
    HttpClientImpl.HttpClientUploadDataProvider uploader =
        new HttpClientImpl.HttpClientUploadDataProvider(data);
    uploader.rewind(mockDataSink);
    verify(mockDataSink).onRewindSucceeded();
  }

  @Test
  public void testHttpClient_chunkedResponse() throws Exception {
    HttpRequest httpRequest = createHttpRequestWithQueryParams();
    HttpOptions httpOptions = createHttpOptions(HttpOptions.HttpMethod.GET);

    setupCronetEngineUrlRequestBuilderMocks();
    byte[] expectedResponseBytes =
        "large-sized-test-response-which-needs-to-be-read-in-chunks".getBytes(UTF_8);
    addHttpStatusAndContentLength(200, -1 /* don't add content length */);

    // Verify request creation and send.
    Future<HttpResponse> future = rpc.send(httpRequest, httpOptions);
    ArgumentCaptor<UrlRequest.Callback> callbackCaptor =
        ArgumentCaptor.forClass(UrlRequest.Callback.class);
    ArgumentCaptor<ByteBuffer> byteBufferCaptor = ArgumentCaptor.forClass(ByteBuffer.class);
    verifyCreateRequestAndSend(EXPECTED_GET_TEST_URL, "GET", callbackCaptor);

    // Send chunked data.
    // Prepare byte buffers to send serialized response chunked into 3 pieces.
    int remainingSize = expectedResponseBytes.length;
    int readIncrement = expectedResponseBytes.length / 3;
    UrlRequest.Callback cronetCallback = callbackCaptor.getValue();

    // Response started, send 1st chunk.
    cronetCallback.onResponseStarted(mockUrlRequest, mockUrlResponseInfo);
    verify(mockUrlRequest, times(1)).read(byteBufferCaptor.capture());
    ByteBuffer byteBuffer1 = byteBufferCaptor.getValue();
    byteBuffer1.put(expectedResponseBytes, 0, readIncrement);
    reset(mockUrlRequest);

    // Read 1st chunk, send 2nd chunk.
    cronetCallback.onReadCompleted(mockUrlRequest, mockUrlResponseInfo, byteBuffer1);
    verify(mockUrlRequest, times(1)).read(byteBufferCaptor.capture());
    ByteBuffer byteBuffer2 = byteBufferCaptor.getValue();
    byteBuffer2.put(expectedResponseBytes, readIncrement, readIncrement);
    reset(mockUrlRequest);

    // Read 2nd chunk, send 3rd chunk.
    cronetCallback.onReadCompleted(mockUrlRequest, mockUrlResponseInfo, byteBuffer2);
    verify(mockUrlRequest, times(1)).read(byteBufferCaptor.capture());
    ByteBuffer byteBuffer3 = byteBufferCaptor.getValue();
    remainingSize -= 2 * readIncrement;
    byteBuffer3.put(expectedResponseBytes, 2 * readIncrement, remainingSize);
    reset(mockUrlRequest);

    // Read 3rd chunk, send onSuccess.
    cronetCallback.onReadCompleted(mockUrlRequest, mockUrlResponseInfo, byteBuffer3);
    cronetCallback.onSucceeded(mockUrlRequest, mockUrlResponseInfo);

    // Verify response bytes in HttpResponse match the expected response.
    HttpResponse response = future.get();
    assertThat(response.getResponse().toByteArray()).isEqualTo(expectedResponseBytes);
  }

  @Test
  public void testHttpClient_responseWithContentLength() throws Exception {
    HttpRequest httpRequest = createHttpRequestWithQueryParams();
    HttpOptions httpOptions = createHttpOptions(HttpOptions.HttpMethod.GET);

    setupCronetEngineUrlRequestBuilderMocks();
    byte[] expectedResponseBytes = "large-sized-test-response".getBytes(UTF_8);
    addHttpStatusAndContentLength(200, expectedResponseBytes.length /* add content length */);

    // Verify request creation and send.
    Future<HttpResponse> future = rpc.send(httpRequest, httpOptions);
    ArgumentCaptor<UrlRequest.Callback> callbackCaptor =
        ArgumentCaptor.forClass(UrlRequest.Callback.class);
    verifyCreateRequestAndSend(EXPECTED_GET_TEST_URL, "GET", callbackCaptor);

    // Execute the response.
    UrlRequest.Callback cronetCallback = callbackCaptor.getValue();
    cronetCallback.onResponseStarted(mockUrlRequest, mockUrlResponseInfo);

    // Verify the size of the byte buffer sent in urlRequest.read matches the content length + 1.
    ArgumentCaptor<ByteBuffer> byteBufferCaptor = ArgumentCaptor.forClass(ByteBuffer.class);
    verify(mockUrlRequest).read(byteBufferCaptor.capture());
    ByteBuffer byteBuffer = byteBufferCaptor.getValue();
    assertThat(byteBuffer.limit()).isEqualTo(expectedResponseBytes.length + 1);

    // Simulate full response is received.
    byteBuffer.put(expectedResponseBytes);
    cronetCallback.onReadCompleted(mockUrlRequest, mockUrlResponseInfo, byteBuffer);
    cronetCallback.onSucceeded(mockUrlRequest, mockUrlResponseInfo);

    // Verify response bytes in HttpResponse match the expected response.
    HttpResponse response = future.get();
    assertThat(response.getResponse().toByteArray()).isEqualTo(expectedResponseBytes);
  }

  @Test
  public void testHttpClient_responseWithDefaultLength() throws Exception {
    HttpRequest httpRequest = createHttpRequestWithQueryParams();
    HttpOptions httpOptions = createHttpOptions(HttpOptions.HttpMethod.GET);

    setupCronetEngineUrlRequestBuilderMocks();
    byte[] expectedResponseBytes = "large-sized-test-response".getBytes(UTF_8);
    addHttpStatusAndContentLength(200, -1 /* don't add content length */);

    // Verify request creation and send.
    Future<HttpResponse> future = rpc.send(httpRequest, httpOptions);
    ArgumentCaptor<UrlRequest.Callback> callbackCaptor =
        ArgumentCaptor.forClass(UrlRequest.Callback.class);
    verifyCreateRequestAndSend(EXPECTED_GET_TEST_URL, "GET", callbackCaptor);

    // Execute the response.
    UrlRequest.Callback cronetCallback = callbackCaptor.getValue();
    cronetCallback.onResponseStarted(mockUrlRequest, mockUrlResponseInfo);

    // Verify the size of the byte buffer sent in urlRequest.read matches the content length + 1.
    ArgumentCaptor<ByteBuffer> byteBufferCaptor = ArgumentCaptor.forClass(ByteBuffer.class);
    verify(mockUrlRequest).read(byteBufferCaptor.capture());
    ByteBuffer byteBuffer = byteBufferCaptor.getValue();
    assertThat(byteBuffer.limit())
        .isEqualTo(HttpClientImpl.HttpUrlRequestCallback.BYTE_BUFFER_LENGTH);

    // Simulate full response is received.
    byteBuffer.put(expectedResponseBytes);
    cronetCallback.onReadCompleted(mockUrlRequest, mockUrlResponseInfo, byteBuffer);
    cronetCallback.onSucceeded(mockUrlRequest, mockUrlResponseInfo);

    // Verify response bytes in HttpResponse match the expected response.
    HttpResponse response = future.get();
    assertThat(response.getResponse().toByteArray()).isEqualTo(expectedResponseBytes);
  }

  @Test
  public void testHttpClient_responseWithHTTPErrorCode() throws Exception {
    HttpRequest httpRequest = createHttpRequestWithQueryParams();
    HttpOptions httpOptions = createHttpOptions(HttpOptions.HttpMethod.GET);

    setupCronetEngineUrlRequestBuilderMocks();
    addHttpStatusAndContentLength(500 /* server error */, -1 /* don't add content length */);

    // Verify request creation and send.
    Future<HttpResponse> future = rpc.send(httpRequest, httpOptions);
    ArgumentCaptor<UrlRequest.Callback> callbackCaptor =
        ArgumentCaptor.forClass(UrlRequest.Callback.class);
    verifyCreateRequestAndSend(EXPECTED_GET_TEST_URL, "GET", callbackCaptor);

    // Execute the response.
    UrlRequest.Callback cronetCallback = callbackCaptor.getValue();
    cronetCallback.onResponseStarted(mockUrlRequest, mockUrlResponseInfo);
    cronetCallback.onReadCompleted(
        mockUrlRequest, mockUrlResponseInfo, ByteBuffer.wrap(new byte[0]));
    cronetCallback.onSucceeded(mockUrlRequest, mockUrlResponseInfo);

    // Verify exception is thrown when future.get() is called.
    try {
      future.get();
      fail("Expecting an exception, but it is not thrown");
    } catch (ExecutionException expected) {
      assertThat(expected).hasCauseThat().isInstanceOf(HttpException.class);
    }
  }

  @Test
  public void testHttpClient_numRedirectsAllowed() throws Exception {
    HttpRequest httpRequest = createHttpRequestWithQueryParams();
    HttpOptions httpOptions = createHttpOptions(HttpOptions.HttpMethod.GET);

    // Set redirect count
    httpOptions = httpOptions.toBuilder().setRedirectsToFollow(2).build();

    setupCronetEngineUrlRequestBuilderMocks();
    addHttpStatusAndContentLength(200, -1);

    Future<HttpResponse> future = rpc.send(httpRequest, httpOptions);
    ArgumentCaptor<UrlRequest.Callback> callbackCaptor =
        ArgumentCaptor.forClass(UrlRequest.Callback.class);
    verifyCreateRequestAndSend(EXPECTED_GET_TEST_URL, "GET", callbackCaptor);

    // Execute the response.
    UrlRequest.Callback cronetCallback = callbackCaptor.getValue();
    cronetCallback.onRedirectReceived(mockUrlRequest, mockUrlResponseInfo, "firstRedirectUrl");
    cronetCallback.onRedirectReceived(mockUrlRequest, mockUrlResponseInfo, "secondRedirectUrl");

    verify(mockUrlRequest, times(2)).followRedirect();
    verify(mockUrlRequest, never()).cancel();
    assertThat(future.isDone()).isFalse();
  }

  @Test
  public void testHttpClient_numRedirectsExceedAllowed() throws Exception {
    HttpRequest httpRequest = createHttpRequestWithQueryParams();
    HttpOptions httpOptions = createHttpOptions(HttpOptions.HttpMethod.GET);

    // Set redirect count to 1
    httpOptions = httpOptions.toBuilder().setRedirectsToFollow(1).build();

    setupCronetEngineUrlRequestBuilderMocks();
    addHttpStatusAndContentLength(200, -1);

    Future<HttpResponse> future = rpc.send(httpRequest, httpOptions);
    ArgumentCaptor<UrlRequest.Callback> callbackCaptor =
        ArgumentCaptor.forClass(UrlRequest.Callback.class);
    verifyCreateRequestAndSend(EXPECTED_GET_TEST_URL, "GET", callbackCaptor);

    // Execute the response.
    UrlRequest.Callback cronetCallback = callbackCaptor.getValue();
    cronetCallback.onRedirectReceived(mockUrlRequest, mockUrlResponseInfo, "firstRedirectUrl");
    cronetCallback.onRedirectReceived(mockUrlRequest, mockUrlResponseInfo, "secondRedirectUrl");

    verify(mockUrlRequest, times(1)).followRedirect();
    verify(mockUrlRequest, times(1)).cancel();
    // Verify that an appropriate exception is thrown when future.get() is called.
    try {
      future.get();
      fail("Expecting an exception, but it is not thrown");
    } catch (ExecutionException expected) {
      assertThat(expected).hasCauseThat().isInstanceOf(HttpException.class);
    }
  }
}
