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
import org.generativecomputing.Intrinsic;
import org.generativecomputing.Struct;
import org.generativecomputing.Value;
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
  private static final String OPEN_AI_REQUEST = "test open ai request";
  private static final String OPEN_AI_RESPONSE = "test open ai response";
  private static final String JSON_REQUEST_TEMPLATE =
      "{\"model\":\"gpt-3.5-turbo\",\"messages\":[{\"role\":\"system\","
          + "\"content\":\"system test content\"},{\"role\":\"user\","
          + "\"content\":\"prompt replaces this\"}]}";

  private static final String EXPECTED_REQUEST =
      "{\"model\":\"gpt-3.5-turbo\",\"messages\":[{\"role\":\"system\","
          + "\"content\":\"system test content\"},{\"role\":\"user\","
          + "\"content\":\"test open ai request\"}]}";

  @Before
  public void setUp() {
    openAiClient = new OpenAiClient(mockHttpClient);
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
    HttpRequest expectedHttpRequest = createExpectedHttpRequestWithPostBody(EXPECTED_REQUEST);
    HttpOptions expectedHttpOptions = createExpectedHttpOptions();
    byte[] expectedResponseBytes = OPEN_AI_RESPONSE.getBytes(UTF_8);
    HttpResponse responseProto =
        HttpResponse.newBuilder().setResponse(ByteString.copyFrom(expectedResponseBytes)).build();
    SettableFuture<HttpResponse> settableFuture = SettableFuture.create();
    settableFuture.set(responseProto);
    when(mockHttpClient.send(any(), any())).thenReturn(settableFuture);

    Struct modelConfig =
        Struct.newBuilder()
            .addElement(Value.newBuilder().setLabel("endpoint").setStr(TEST_URL))
            .addElement(Value.newBuilder().setLabel("api_key").setStr(TEST_API_KEY))
            .addElement(
                Value.newBuilder().setLabel("json_request_template").setStr(JSON_REQUEST_TEMPLATE))
            .build();
    Struct intrinsicStruct =
        Struct.newBuilder()
            .addElement(Value.newBuilder().setLabel("model_uri").setStr("test_model_uri"))
            .addElement(Value.newBuilder().setLabel("model_config").setStruct(modelConfig))
            .build();
    Value modelConfigProto =
        Value.newBuilder()
            .setIntrinsic(
                Intrinsic.newBuilder()
                    .setUri("model_inference_with_config")
                    .setStaticParameter(Value.newBuilder().setStruct(intrinsicStruct)))
            .build();

    String response =
        openAiClient.call(modelConfigProto.toByteArray(), OPEN_AI_REQUEST.getBytes(UTF_8));

    ArgumentCaptor<HttpRequest> httpRequestCaptor = ArgumentCaptor.forClass(HttpRequest.class);
    ArgumentCaptor<HttpOptions> httpOptionsCaptor = ArgumentCaptor.forClass(HttpOptions.class);

    verify(mockHttpClient).send(httpRequestCaptor.capture(), httpOptionsCaptor.capture());
    assertThat(httpRequestCaptor.getValue()).isEqualTo(expectedHttpRequest);
    assertThat(httpOptionsCaptor.getValue()).isEqualTo(expectedHttpOptions);
    assertThat(response).isEqualTo(OPEN_AI_RESPONSE);
  }
}
