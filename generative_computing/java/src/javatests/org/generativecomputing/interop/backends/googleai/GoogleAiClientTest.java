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

package src.javatests.org.generativecomputing.interop.backends.googleai;

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
import src.java.org.generativecomputing.interop.backends.googleai.GoogleAiClient;
import src.java.org.generativecomputing.interop.network.HttpClientImpl;
import src.java.org.generativecomputing.interop.network.api.proto.HttpOptions;
import src.java.org.generativecomputing.interop.network.api.proto.HttpRequest;
import src.java.org.generativecomputing.interop.network.api.proto.HttpResponse;

/** Unit tests for GoogleAiClient. */
@RunWith(AndroidJUnit4.class)
public final class GoogleAiClientTest {
  @Rule public final MockitoRule mocks = MockitoJUnit.rule();

  @Mock private HttpClientImpl mockHttpClient;
  private GoogleAiClient googleAiClient;
  private static final String TEST_URL =
      "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent";
  private static final String KEY_QUERY_PARAM = "key";
  private static final String TEST_API_KEY = "test-api-key";
  private static final String CONTENT_TYPE_HEADER = "Content-Type";
  private static final String TEST_CONTENT_TYPE = "application/json";
  public static final String KEY_AUTHORIZATION = "Authorization";
  private static final String AUTHORIZATION_HEADER_PREFIX = "Bearer ";
  private static final String TEST_ACCESS_TOKEN = "test-access-token";
  private static final String TEST_REQUEST = "tell me more";
  private static final String TEST_RESPONSE = "test-response-data";
  private static final String JSON_REQUEST_TEMPLATE =
      "{\"contents\":[{\"role\":\"user\",\"parts\":[{\"text\":\"Write the first line of a"
          + " story about a magic backpack.\"}]},{\"role\": \"model\",\"parts\":[{\"text\":"
          + "\"In the bustling city of Meadow brook, lived a young girl named Sophie. She was a"
          + " bright and curious soul with an imaginative mind.\"}]},{\"role\": \"user\","
          + "\"parts\":[{\"text\":\"\"}]}]}";
  private static final String EXPECTED_REQUEST =
      "{\"contents\":[{\"role\":\"user\",\"parts\":[{\"text\":\"Write the first line of a story"
          + " about a magic backpack.\"}]},{\"role\":\"model\",\"parts\":[{\"text\":\"In the"
          + " bustling city of Meadow brook, lived a young girl named Sophie. She was a bright and"
          + " curious soul with an imaginative"
          + " mind.\"}]},{\"role\":\"user\",\"parts\":[{\"text\":\"tell me more\"}]}]}";

  @Before
  public void setUp() {
    googleAiClient = new GoogleAiClient(mockHttpClient);
  }

  private HttpOptions createExpectedHttpOptions(boolean accessTokenPresent) {
    HttpOptions.Builder httpOptionsBuilder =
        HttpOptions.newBuilder().setUrl(TEST_URL).setHttpMethod(HttpOptions.HttpMethod.POST);
    httpOptionsBuilder.addHttpHeaders(
        HttpOptions.HttpHeader.newBuilder()
            .setName(CONTENT_TYPE_HEADER)
            .setValue(TEST_CONTENT_TYPE)
            .build());
    if (accessTokenPresent) {
      httpOptionsBuilder.addHttpHeaders(
          HttpOptions.HttpHeader.newBuilder()
              .setName(KEY_AUTHORIZATION)
              .setValue(AUTHORIZATION_HEADER_PREFIX + TEST_ACCESS_TOKEN)
              .build());
    }
    return httpOptionsBuilder.build();
  }

  private HttpRequest createExpectedHttpRequestWithPostBody(
      String postBody, boolean apiKeyPresent) {
    HttpRequest.Builder httpRequestBuilder = HttpRequest.newBuilder();
    if (apiKeyPresent) {
      httpRequestBuilder.addQueryParams(
          HttpRequest.QueryParam.newBuilder()
              .setParam(KEY_QUERY_PARAM)
              .setValue(TEST_API_KEY)
              .build());
    }
    httpRequestBuilder.setPostBody(
        HttpRequest.PostBody.newBuilder().setData(ByteString.copyFromUtf8(postBody)).build());
    return httpRequestBuilder.build();
  }

  @Test
  public void googleAiClient_successfulCallWithApiKey() {
    HttpRequest expectedHttpRequest = createExpectedHttpRequestWithPostBody(EXPECTED_REQUEST, true);
    HttpOptions expectedHttpOptions = createExpectedHttpOptions(false);
    byte[] expectedResponseBytes = TEST_RESPONSE.getBytes(UTF_8);
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
        googleAiClient.call(modelConfigProto.toByteArray(), TEST_REQUEST.getBytes(UTF_8));

    ArgumentCaptor<HttpRequest> httpRequestCaptor = ArgumentCaptor.forClass(HttpRequest.class);
    ArgumentCaptor<HttpOptions> httpOptionsCaptor = ArgumentCaptor.forClass(HttpOptions.class);

    verify(mockHttpClient).send(httpRequestCaptor.capture(), httpOptionsCaptor.capture());
    assertThat(httpRequestCaptor.getValue()).isEqualTo(expectedHttpRequest);
    assertThat(httpOptionsCaptor.getValue()).isEqualTo(expectedHttpOptions);
    assertThat(response).isEqualTo(TEST_RESPONSE);
  }

  @Test
  public void googleAiClient_successfulCallWithAccessToken() {
    HttpRequest expectedHttpRequest =
        createExpectedHttpRequestWithPostBody(EXPECTED_REQUEST, false);
    HttpOptions expectedHttpOptions = createExpectedHttpOptions(true);
    byte[] expectedResponseBytes = TEST_RESPONSE.getBytes(UTF_8);
    HttpResponse responseProto =
        HttpResponse.newBuilder().setResponse(ByteString.copyFrom(expectedResponseBytes)).build();
    SettableFuture<HttpResponse> settableFuture = SettableFuture.create();
    settableFuture.set(responseProto);
    when(mockHttpClient.send(any(), any())).thenReturn(settableFuture);

    Struct modelConfig =
        Struct.newBuilder()
            .addElement(Value.newBuilder().setLabel("endpoint").setStr(TEST_URL))
            .addElement(Value.newBuilder().setLabel("access_token").setStr(TEST_ACCESS_TOKEN))
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
        googleAiClient.call(modelConfigProto.toByteArray(), TEST_REQUEST.getBytes(UTF_8));

    ArgumentCaptor<HttpRequest> httpRequestCaptor = ArgumentCaptor.forClass(HttpRequest.class);
    ArgumentCaptor<HttpOptions> httpOptionsCaptor = ArgumentCaptor.forClass(HttpOptions.class);

    verify(mockHttpClient).send(httpRequestCaptor.capture(), httpOptionsCaptor.capture());
    assertThat(httpRequestCaptor.getValue()).isEqualTo(expectedHttpRequest);
    assertThat(httpOptionsCaptor.getValue()).isEqualTo(expectedHttpOptions);
    assertThat(response).isEqualTo(TEST_RESPONSE);
  }
}
