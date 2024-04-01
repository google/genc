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

package src.javatests.org.genc.interop.backends.wolframalpha;

import static com.google.common.truth.Truth.assertThat;
import static java.nio.charset.StandardCharsets.UTF_8;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import com.google.common.util.concurrent.SettableFuture;
import com.google.protobuf.ByteString;
import org.genc.Intrinsic;
import org.genc.Value;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;
import org.mockito.ArgumentCaptor;
import org.mockito.Mock;
import org.mockito.junit.MockitoJUnit;
import org.mockito.junit.MockitoRule;
import src.java.org.genc.interop.backends.wolframalpha.WolframAlphaClient;
import src.java.org.genc.interop.network.HttpClient;
import src.java.org.genc.interop.network.api.proto.HttpOptions;
import src.java.org.genc.interop.network.api.proto.HttpRequest;
import src.java.org.genc.interop.network.api.proto.HttpResponse;

/** Unit tests for WolframAlphaClient. */
@RunWith(JUnit4.class)
public final class WolframAlphaClientTest {
  @Rule public final MockitoRule mocks = MockitoJUnit.rule();

  @Mock private HttpClient mockHttpClient;
  private WolframAlphaClient wolframAlphaClient;
  private static final String EXPECTED_URL = "https://api.wolframalpha.com/v2/query";
  private static final String EXPECTED_KEY_APPID = "appid";
  private static final String EXPECTED_KEY_INPUT = "input";
  private static final String EXPECTED_KEY_OUTPUT = "output";
  private static final String EXPECTED_OUTPUT_VALUE = "json";
  private static final String EXPECTED_KEY_INCLUDE_POD_ID = "includepodid";
  private static final String EXPECTED_INCLUDE_POD_ID_VALUE = "Result";

  private static final String TEST_APP_ID = "test-app-id";
  private static final String TEST_REQUEST = "solve 2 + 2 ^ 5";
  private static final String TEST_RESPONSE = "34";

  @Before
  public void setUp() {
    wolframAlphaClient = new WolframAlphaClient(mockHttpClient);
  }

  private HttpOptions createExpectedHttpOptions() {
    HttpOptions httpOptions =
        HttpOptions.newBuilder()
            .setUrl(EXPECTED_URL)
            .setHttpMethod(HttpOptions.HttpMethod.GET)
            .build();
    return httpOptions;
  }

  private HttpRequest createExpectedHttpRequest() {
    HttpRequest.Builder httpRequestBuilder = HttpRequest.newBuilder();
    httpRequestBuilder.addQueryParams(
        HttpRequest.QueryParam.newBuilder()
            .setParam(EXPECTED_KEY_APPID)
            .setValue(TEST_APP_ID)
            .build());
    httpRequestBuilder.addQueryParams(
        HttpRequest.QueryParam.newBuilder()
            .setParam(EXPECTED_KEY_OUTPUT)
            .setValue(EXPECTED_OUTPUT_VALUE)
            .build());
    httpRequestBuilder.addQueryParams(
        HttpRequest.QueryParam.newBuilder()
            .setParam(EXPECTED_KEY_INCLUDE_POD_ID)
            .setValue(EXPECTED_INCLUDE_POD_ID_VALUE)
            .build());
    httpRequestBuilder.addQueryParams(
        HttpRequest.QueryParam.newBuilder()
            .setParam(EXPECTED_KEY_INPUT)
            .setValue(TEST_REQUEST)
            .build());
    return httpRequestBuilder.build();
  }

  @Test
  public void wolframAlphaClient_successfulCall() {
    HttpRequest expectedHttpRequest = createExpectedHttpRequest();
    HttpOptions expectedHttpOptions = createExpectedHttpOptions();
    byte[] expectedResponseBytes = TEST_RESPONSE.getBytes(UTF_8);
    HttpResponse responseProto =
        HttpResponse.newBuilder().setResponse(ByteString.copyFrom(expectedResponseBytes)).build();
    SettableFuture<HttpResponse> settableFuture = SettableFuture.create();
    settableFuture.set(responseProto);
    when(mockHttpClient.send(any(), any())).thenReturn(settableFuture);

    Value configProto =
        Value.newBuilder()
            .setIntrinsic(
                Intrinsic.newBuilder()
                    .setUri("wolfram_alpha")
                    .setStaticParameter(Value.newBuilder().setStr(TEST_APP_ID)))
            .build();

    String response =
        wolframAlphaClient.call(configProto.toByteArray(), TEST_REQUEST.getBytes(UTF_8));

    ArgumentCaptor<HttpRequest> httpRequestCaptor = ArgumentCaptor.forClass(HttpRequest.class);
    ArgumentCaptor<HttpOptions> httpOptionsCaptor = ArgumentCaptor.forClass(HttpOptions.class);

    verify(mockHttpClient).send(httpRequestCaptor.capture(), httpOptionsCaptor.capture());
    assertThat(httpRequestCaptor.getValue()).isEqualTo(expectedHttpRequest);
    assertThat(httpOptionsCaptor.getValue()).isEqualTo(expectedHttpOptions);
    assertThat(response).isEqualTo(TEST_RESPONSE);
  }
}
