# GenC Models

Note: **Disclaimer**: The GenC Framework is experimental and under active
development. It is currently intended for research and experimental purposes,
and uses in a non-experimental setting are at your own risk. Additionally, use
of the GenC Framework is subject to the [Generative AI Prohibited Use Policy](https://policies.google.com/terms/generative-ai/use-policy).

Note: You are responsible for complying with any applicable license terms when
using any of the models below.

This doc covers the list of models currently supported by code included in the
GenC repo (note you can always configure the runtime to support your own custom
model backends; see section on extensibility in [api.md](api.md)).

## Cloud models

GenC provides out-of-box connections with most cloud model services. We support
following backends across all currently supported platforms and one can author
GenAI workflows leveraging these in Python, C++, and Java:

* [Google AI Studio](https://ai.google.dev/tutorials/rest_quickstart). The [tutorials](tutorials) also use AI studio.
* [Google Vertex AI](https://cloud.google.com/vertex-ai)
* [OpenAI](https://platform.openai.com/docs/guides/text-generation/chat-completions-api)

In our examples, we use Gemini-Pro, and OpenAI ChatGPT for chat completions,
however, the model inference setup is configurable to call other models
supported in above backends and/or other backends \(see [api.md](api.md)\).

Below are the set up steps for each of the above backends:

### [Google AI Studio](https://ai.google.dev/tutorials/rest_quickstart)
We use the REST API to call model inferences on Gemini-Pro model. The
configuration includes ```API key```, ```endpoint URL```, and
```JSON request template```. Please [see instructions](https://ai.google.dev/tutorials/rest_quickstart)
to get your API key from Google Studio. The ```endpoint URL``` is ```https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent```
and the ```JSON request template``` contains the model parameters and safety
settings per the Gemini model's [configuration options](https://ai.google.dev/tutorials/rest_quickstart#configuration).

In our example demos, we have seeded the JSON request template with a [default
configuration](../python/interop/gemini/create_config.py), however, please feel
free to use different template per your use-case.

#### Python
Example code snippets to illustrate model inference construction in Python:

1) Using defaults for some customizations:

```
api_key = ...
gemini_model = genc.interop.langchain.CustomModel(
    uri="/cloud/gemini",
    config=genc.interop.gemini.create_config(api_key))
```

2) Fully specified customizations:

```
api_key = ...
endpoint_url = ...
json_request_template = ...
gemini_model = genc.interop.langchain.CustomModel(
    uri="/cloud/gemini",
    config=genc.interop.gemini.create_config(
      api_key = api_key,
      endpoint = endpoint_url,
      json_request_template = json_request_template))
```

See [Tutorial 1](tutorials/tutorial_1_simple_cascade.ipynb) for demo, it uses
Gemini-Pro via Google AI Studio for cloud model.

In our ```DefaultLocalExecutor``` used in demos, the handler that processes
the model inference calls for Gemini-Pro and calls the cloud backend is
registered at the uri ```/cloud/gemini```. You can change this uri string to
another preference, if you do so, change the model inference handler
registration to that chosen string in the executor stacks \(see [api.md](api.md)
for details\).

#### Java
In Java, you can create the model inference as follows; additionally please see
demo at [GeminiOnAiStudioDemo.java](../java/src/java/org/genc/examples/GeminiOnAiStudioDemo.java)

```
Value modelConfig =
    Constructor.createGeminiModelConfigForGoogleAiStudio(
      /* apiKey= */ apiKey,
      /* endpoint= */ endpoint,
      /* requestJsonTemplate= */ jsonRequestTemplate);

Value geminiModel =
    Constructor.createModelInferenceWithConfig("/cloud/gemini", modelConfig);
```

#### C++
For C++, see example code snippet below:

```
v0::Value model_config = CreateRestModelConfig(end_point, api_key);
v0::Value model_call =
      GENC_TRY(CreateModelInferenceWithConfig("/cloud/gemini", model_config))
```
Additionally, see demo at [run_gemini_on_ai_studio](../cc/examples/run_gemini_on_ai_studio.cc)

### [Google Vertex AI](https://cloud.google.com/vertex-ai)

To authenticate to Vertex AI, first you need to get an access token. Please see
instructions in [Authenticate to Vertex AI](https://cloud.google.com/vertex-ai/docs/authentication#authn-how-to)
to set up your access token. Since we use the REST API in GenC to connect to
Vertex AI, getting an access token through
[these instructions](https://cloud.google.com/vertex-ai/docs/authentication#rest)
would suffice.

For endpoint, we will be using ```streamGenerateContent``` endpoint on VertexAI.
The full endpoint URL looks something like this:

```https://{REGION}-aiplatform.googleapis.com/v1/projects/{PROJECT_ID}/locations/{REGION}/publishers/google/models/gemini-1.0-pro:streamGenerateContent```.

Please see
[Vertex AI Gemini reference guide](https://cloud.google.com/vertex-ai/generative-ai/docs/model-reference/gemini)
on different regions supported
\(we will be using ```us-central1``` as the region in our demos\).
The ```PROJECT_ID``` is the ID of your Google Cloud project. If you are new to
Google Cloud, please see [Get set up on Google Cloud](https://cloud.google.com/vertex-ai/docs/start/cloud-environment) and [this quickstart guide](https://cloud.google.com/vertex-ai/generative-ai/docs/start/quickstarts/quickstart-multimodal)
with details on how to set up a Google Cloud account, enable Vertex AI API and
other useful information.

Similar to Google Studio, the Gemini-Pro API allows developers to specify
```generationConfig```, ```safetySetting``` and other customization options in
each request. See [Gemini API reference](https://cloud.google.com/vertex-ai/generative-ai/docs/model-reference/gemini)
for settings and API documentation. In GenC, we take ```json_request_template```
as a configuration param in model inference definition for developers to be able
to fully specify the request except for the dynamic input prompt.

#### Python
For model inference creation in Python, see below:

```
access_token = ...
endpoint = ...
json_request_template = ...
gemini_model = genc.python.interop.langchain.CustomModel(
    uri="/cloud/gemini",
    config= {'access_token': access_token, 'endpoint': endpoint,
             'json_request_template': json_request_template})
```

#### Java
For model inference creation in Java, see below.

```
Value vertexAiGeminiModel = Constructor.createModelInferenceWithConfig(
    "/cloud/gemini",
    Constructor.createGeminiModelConfigForVertexAi(
      accessToken, endpoint, jsonRequestTemplate));
```

#### C++
See below for code snippet for C++; additionally please see demo at
[run_gemini_on_vertex.cc](../cc/examples/run_gemini_on_vertex.cc)

```
absl::StatusOr<v0::Value> RunGeminiOnVertex(std::string json_request) {
  std::string access_token = absl::GetFlag(access_token);
  std::shared_ptr<Executor> executor = GENC_TRY(CreateDefaultExecutor());
  v0::Value rest_call = GENC_TRY(CreateRestCall(endPoint, access_token));
  ...
```

### [OpenAI](https://platform.openai.com/docs/guides/text-generation/chat-completions-api)
In GenC, we have integrated with OpenAI's Chat completions API to use in LLM
pipelines and demos. To use this backend, you need to provide an API key, see
instructions in [OpenAI's quickstart guide](https://platform.openai.com/docs/quickstart)
on how to set up an account and create an API key.

Additionally, please familiarize yourself with the [Chat completions API](https://platform.openai.com/docs/api-reference/chat/create),
particularly the settings in the ```request body```. In GenC, we use
```json_request_template``` to fill in configuration settings in
```request body``` and as part of GenC pipeline run, the last message is
appended dynamically. You can create a custom
```json_request_template``` per your liking.

The endpoint for OpenAI chat completions is ```https://api.openai.com/v1/chat/completions```

#### Python
Below is example code snippet for creating OpenAI backed model inference in
Python. See full example at [OpenAI Demo](../python/examples/openai_demo.py):

```
  api_key = ...
  endpoint = "https://api.openai.com/v1/chat/completions"
  model_call = authoring.create_rest_call(endpoint, api_key)
  runner = runtime.Runner(model_call, executor.create_default_executor())
  request = pb.Value()
  request.str = (
      b'{"model": "gpt-3.5-turbo", "messages": [{"role": "user", "content":'
      b' <PROMPT_QUERY>}]}'
  )
  result = json.loads(comp(request))
```

#### Java
In Java, you can create OpenAI model inference as follows; additionally please
see demo at [OpenAiDemo.java](../java/src/java/org/genc/examples/OpenAiDemo.java).

```
// Create Open AI chat completions model inference.
apiKey = ...
jsonRequestTemplate = ...
endpoint = "https://api.openai.com/v1/chat/completions"
Value openAiModel = Constructor.createModelInferenceWithConfig(
            "/openai/chatgpt", Constructor.createOpenAiModelConfig(
      apiKey, endpoint, jsonRequestTemplate));
```

#### C++
For C++, see example code snippet below; additionally see demo at
[run_openai.cc](../cc/examples/run_openai.cc)

```
v0::Value rest_call = GENC_TRY(CreateRestCall(endpoint, api_key));
```

## On-device models

Currently, two backends are provided to facilitate access to on-device models.

### MediaPipe
Note: The Mediapipe LLM Inference APIs are experimental and under development.
It is currently intended for research and experimental purposes. If you're
planning to build a production mobile app to run on Android, at this time we
recommend that you review the [Gemini API](https://ai.google.dev/),
and the
[Gemini Nano on-device through Android AICore](https://android-developers.googleblog.com/2023/12/a-new-foundation-for-ai-on-android.html). AICore is the new system-level
capability introduced in Android 14 to provide Gemini-powered solutions
for high-end devices, including integrations with the
latest ML accelerators, use-case optimized LoRA adapters, and safety filters.
To start using Gemini Nano on-device with your app, apply to the
[Early Access Preview](https://docs.google.com/forms/d/e/1FAIpQLSdDvg0eEzcUY_-CmtiMZLd68KD3F0usCnRzKKzWb4sAYwhFJg/viewform?usp=header_link).

[MediaPipe](https://developers.google.com/mediapipe) is a cross-platform
framework and collection of solutions for optimized on-device ML. MediaPipe
recently launched [LLM Inference API](https://developers.google.com/mediapipe/solutions/genai/llm_inference)
to run large language models (LLMs) completely on-device. As of this writing,
LLM Inference API supports following models:

* [Gemma 2B](https://ai.google.dev/gemma/docs)
* [Phi-2](https://huggingface.co/microsoft/phi-2)
* [Falcon-RW-1B](https://huggingface.co/tiiuae/falcon-rw-1b)
* [StableLM-3B](https://huggingface.co/stabilityai/stablelm-3b-4e1t)

GenC connects with MediaPipe LLM Inference API to offer on-device model
inferences in a GenAI pipeline workflow.

MediaPipe LLM Inference integration is currently available on Android. We also
plan to enable iOS, Web integrations as part of GenC on iOS and Web in
upcoming future.

#### Download Model to Device
##### Download Model
Please see instructions at [Models](https://developers.google.com/mediapipe/solutions/genai/llm_inference#models)
section on the MediaPipe’s LLM Inference site to download one or more of the
models.

We recommend using Gemma 2B, available on [Kaggle Models](https://www.kaggle.com/models/google/gemma),
it comes in a format that is already compatible with the LLM Inference API and
can be directly loaded onto your device. If you use another model, you will
need to convert the model to a MediaPipe-friendly format. See following
section for conversion steps.

##### Convert Models to MediaPipe format
Gemma downloaded from non-Kaggle sources and all other external models supported
via MediaPipe (Phi-2, Falcon-RW-1B, StableLM-3B) need to be converted first to
use them with LLM Inference API on device.

See instructions for model conversion in
[the MediaPipe documentation](https://developers.google.com/mediapipe/solutions/genai/llm_inference/android#convert-model)
to download needed MediaPipe package and run model
conversion script. Additionally, for easier conversion flow, you could also
use [Model Conversion Colab](https://colab.sandbox.google.com/github/googlesamples/mediapipe/blob/main/examples/llm_inference/conversion/llm_conversion.ipynb).


##### Push the model to the device

Push the downloaded \(and converted\) model to your device.

See instructions for pushing models in
[the MediaPipe documentation](https://developers.google.com/mediapipe/solutions/genai/llm_inference/android#push_model_to_the_device).
Please take note of the model path used on the
device \(e.g. ```/data/local/tmp/llm/gemma-2b-it-gpu-int4.bin```). We will be using
the model path in the following sections.

##### Instantiate LLM Inference backed model inference in GenC

LLM Inference API supports several configuration options. When instantiating a
model inference in GenC, please provide desired values for maxTokens, topK,
temperature, and randomSeed; alongside the on-device model path.
See the section on configuration options in
[the MediaPipe documentation](https://developers.google.com/mediapipe/solutions/genai/llm_inference/android#configuration_options)
to learn more about each configuration setting.

Following code illustrates how to create a model inference backed by MediaPipe
LLM Inference in Java.

```
// Create MediaPipe LLM Inference backed model inference.
Value mediaPipeLlmInferenceModelConfig =
    Constructor.createMediaPipeLlmInferenceModelConfig(
        /* modelPath= */ "/data/local/tmp/llm/gemma-2b-it-gpu-int4.bin",
        /* maxTokens= */ 64,
        /* topK= */ 40,
        /* temperature= */ 0.8f,
        /* randomSeed= */ 100);

Value mediaPipeLlmInferenceModel =
    Constructor.createModelInferenceWithConfig(
            "/device/llm_inference",
            mediaPipeLlmInferenceModelConfig);
```

##### Use LLM Inference in a chain in GenC

Following code illustrates usage of above ```mediaPipeLlmInferenceModel``` llm
inference model in a simple {prompt, model inference} chain with GenC.

```
// Create prompt template to use.
Value promptTemplate = Constructor.createPromptTemplate("Tell me about {topic}?");

// Create prompt, model inference chain.
return Constructor.createSerialChain(
  new ArrayList<>(ImmutableList.of(promptTemplate, mediaPipeLlmInferenceModel)));
}
```

Additionally, following example illustrates how to instantiate and use
LLM Inference in a LangChain -> on-device deployment. Code is authored in Python
for IR to be deployed on your device:

```
my_chain = chains.LLMChain(
      llm=interop.langchain.CustomModel(uri="/device/llm_inference",
      config={"model_path": "/data/local/tmp/llm/gemma-2b-it-gpu-int4.bin",
              "max_tokens": 64,
              "top_k": 40,
              "temperature": 0.8,
              "random_seed": 100}),
      prompt=prompts.PromptTemplate(
          input_variables=["topic"],
          template="Q: Tell me about {topic}? A: ",
      ),
  )
```

### Llama.cpp

[Llama.cpp](https://github.com/ggerganov/llama.cpp) is a popular OSS framework
that enables LLM inference across many platforms with minimal setup. It
supports a massive collection of models including Gemma, Llama, Mistral,
Phi, Falcon, StableLM, etc (the entire list can be found on the repo).

LlamaCpp includes tools for converting models to its preferred format (GGUF),
but for standard models it is likely easiest to obtain the models directly as
GGUF files from Hugging Face. A
[Hugging Face search of "GGUF"](https://huggingface.co/models?search=gguf)
will yield 1000s of models that can easily be run with Llama.cpp.

Many models will include many quantization options indicating the bits,
quantization technique, and size. For example, one of the most popular options
`Q4_K_M` indicates 4-bit quantization, k-quant (newer GGML quant method),
and medium size.

These models can be downloaded from Hugging Face (using the website, the
[Hugging Face CLI](https://huggingface.co/docs/huggingface_hub/en/guides/cli),
or GitHub LFS) and directly uploaded onto your device. Some models to
consider include:

* [Gemma 2B Instruction Tuned](https://huggingface.co/google/gemma-2b-it)
* [Mistral 7B](https://huggingface.co/TheBloke/Mistral-7B-Instruct-v0.2-GGUF)
* [TinyLlama](https://huggingface.co/TheBloke/TinyLlama-1.1B-Chat-v1.0-GGUF)

When running the model in your GenC code, you'll need to include the absolute
model path, the number of threads Llama.cpp can use, and the max tokens to
generate (responses may end early if a EOS is detected). For example, if
creating the IR in Python to run on-device, the following could be used:

```
my_chain = chains.LLMChain(
      llm=genc.python.interop.langchain.CustomModel(uri="/device/gemma",
      config={"model_path": "/data/local/tmp/gemma-2b-it-q4_k_m.gguf",
            "num_threads": 4,
            "max_tokens": 64}),
      prompt=prompts.PromptTemplate(
          input_variables=["question"],
          template="Q: {question}? A: ",
      ),
  )
```

Note the included example runtime used in the tutorials has Llama.cpp registered
as the handler of the `/device/gemma` model for simplicity's sake, but you can
opt to use a different name, model, or model provider in your custom deployment.
