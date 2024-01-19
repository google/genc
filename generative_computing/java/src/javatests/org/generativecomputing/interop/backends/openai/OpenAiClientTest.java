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
package src.javatests.org.generativecomputing.interop.backends.openai;

import static com.google.common.truth.Truth.assertThat;
import static java.nio.charset.StandardCharsets.UTF_8;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import com.google.common.util.concurrent.SettableFuture;
import com.google.protobuf.ByteString;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.ArgumentCaptor;
import org.mockito.Mock;
import org.mockito.junit.MockitoJUnit;
import org.mockito.junit.MockitoRule;
import src.java.org.generativecomputing.interop.backends.openai.OpenAiClient;
import src.java.org.generativecomputing.interop.network.HttpClientImpl;
import src.java.org.generativecomputing.interop.network.api.proto.HttpOptions;
import src.java.org.generativecomputing.interop.network.api.proto.HttpRequest;
import src.java.org.generativecomputing.interop.network.api.proto.HttpResponse;

/** Unit tests for OpenAiClient. */
@RunWith(AndroidJUnit4.class)
public final class OpenAiClientTest {
  @Rule public final MockitoRule mocks = MockitoJUnit.rule();

  @Mock private HttpClientImpl mockHttpClient;
  private OpenAiClient openAiClient;
  private static final String TEST_URL = "https://api.openai.com/v1/chat/completions";
  private static final String TEST_API_KEY = "test-api-key";
  private static final String CONTENT_TYPE_HEADER = "Content-Type";
  private static final String CONTENT_TYPE = "application/json";
  public static final String KEY_AUTHORIZATION = "Authorization";
  private static final String AUTHORIZATION_HEADER_PREFIX = "Bearer ";
  private static final String OPEN_AIREQUEST = "test-open-ai-request";
  private static final String OPENAI_RESPONSE = "test-open-ai-response-data";

  @Before
  public void setUp() {
    openAiClient = new OpenAiClient(mockHttpClient, TEST_URL, TEST_API_KEY);
  }

  private HttpOptions createExpectedHttpOptions() {
    HttpOptions.Builder httpOptionsBuilder =
        HttpOptions.newBuilder().setUrl(TEST_URL).setHttpMethod(HttpOptions.HttpMethod.POST);
    httpOptionsBuilder.addHttpHeaders(
        HttpOptions.HttpHeader.newBuilder()
            .setName(CONTENT_TYPE_HEADER)
            .setValue(CONTENT_TYPE)
            .build());
    httpOptionsBuilder.addHttpHeaders(
        HttpOptions.HttpHeader.newBuilder()
            .setName(KEY_AUTHORIZATION)
            .setValue(AUTHORIZATION_HEADER_PREFIX + TEST_API_KEY)
            .build());
    return httpOptionsBuilder.build();
  }

  private HttpRequest createExpectedHttpRequestWithPostBody(String postBody) {
    HttpRequest.Builder httpRequestBuilder = HttpRequest.newBuilder();
    httpRequestBuilder.setPostBody(
        HttpRequest.PostBody.newBuilder().setData(ByteString.copyFromUtf8(postBody)).build());
    return httpRequestBuilder.build();
  }

  @Test
  public void openAiClient_successfulCallToHttpClient() {
    HttpRequest expectedHttpRequest = createExpectedHttpRequestWithPostBody(OPEN_AIREQUEST);
    HttpOptions expectedHttpOptions = createExpectedHttpOptions();
    byte[] expectedResponseBytes = OPENAI_RESPONSE.getBytes(UTF_8);
    HttpResponse responseProto =
        HttpResponse.newBuilder().setResponse(ByteString.copyFrom(expectedResponseBytes)).build();
    SettableFuture<HttpResponse> settableFuture = SettableFuture.create();
    settableFuture.set(responseProto);
    when(mockHttpClient.send(any(), any())).thenReturn(settableFuture);

    String response = openAiClient.call(OPEN_AIREQUEST.getBytes(UTF_8));

    ArgumentCaptor<HttpRequest> httpRequestCaptor = ArgumentCaptor.forClass(HttpRequest.class);
    ArgumentCaptor<HttpOptions> httpOptionsCaptor = ArgumentCaptor.forClass(HttpOptions.class);

    verify(mockHttpClient).send(httpRequestCaptor.capture(), httpOptionsCaptor.capture());
    assertThat(httpRequestCaptor.getValue()).isEqualTo(expectedHttpRequest);
    assertThat(httpOptionsCaptor.getValue()).isEqualTo(expectedHttpOptions);
    assertThat(response).isEqualTo(OPENAI_RESPONSE);
  }
}
