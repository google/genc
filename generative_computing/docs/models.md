# Generative Computing (GenC) Models

This doc covers the list of models currently supported by code included in the
GenC repo (note you can always configure the runtime to support your own custom
model backends; see section on extensibility in [api.md](api.md)).

## Cloud models

GenC provides a simple REST interface to allow connection to most cloud model services. We've included examples for the following:

* Google AI Studio (the [tutorials](tutorials) also use AI studio)
* Google Vertex AI
* OpenAI

The process is the same for all of these, obtain an API key from the cloud provider and provide this key as part of the generated IR. This can be accomplished with a config to a model:

```
my_cloud_model = genc.interop.langchain.CustomModel(
    uri="/cloud/gemini",
    config=genc.interop.gemini.create_config(API_KEY))
```

or directly as a REST endpoint

```
// C++
absl::StatusOr<v0::Value> RunGeminiOnVertex(std::string json_request) {
  std::string api_key = absl::GetFlag(FLAGS_api_key);
  std::shared_ptr<Executor> executor = GENC_TRY(CreateDefaultExecutor());
  v0::Value rest_call = GENC_TRY(CreateRestCall(kEndPoint, api_key));
  ...

// Python
model_call = authoring.create_rest_call(endpoint, FLAGS.api_key)
```

Connecting to other endpoints requires updating the endpoint and the API key. Note that some cloud endpoints will expect the request in a certain format. For example, in the [OpenAI Demo](../python/examples/openai_demo.py) a JSON format is created:

```
request.str = (
      b'{"model": "gpt-3.5-turbo", "messages": [{"role": "user", "content":'
      b' "Can you help me know things to do in San Fransisco"}]}'
  )
```


## On-device models

GenC is a bring-your-own-model (BYOM) framework. Currently, two backends are
provided to enable on-device models

### MediaPipe

[MediaPipe](https://developers.google.com/mediapipe) is a cross-platform
framework and collection of solutions for optimized on-device ML. GenC
exposes one such solution: LLM Inference.

As of this writing, MediaPipe supports the following models:

* Gemma
* StableLM
* Falcon
* Phi 2

TODO: Obtaining model info

### Llama.cpp

[Llama.cpp](https://github.com/ggerganov/llama.cpp) is a popular OSS framework
that enables LLM inference across many platforms with minimal setup. It
supports a massive collection of models including Gemma, Llama, Mistral,
Phi, Falcon, StableLM, etc (the entire list can be found on the repo).

LlamaCpp includes tools for converting models to its preferred format (GGUF),
but for standard models it is likely easiest to obtain the models directly as
GGUF files from Hugging Face. A [Hugging Face search of "GGUF"](https://huggingface.co/models?search=gguf)
will yield 1000s of models that can easily be run with LlamaCpp.

Many models will include many quantization options indicating the bits,
quantization technique, and size. For example, one of the most popular options
`Q4_K_M` indicates 4-bit quantization, k-quant (newer GGML quant method),
and medium size.

These models can be downloaded from Hugging Face (using the website, the
[Hugging Face CLI](https://huggingface.co/docs/huggingface_hub/en/guides/cli), or GitHub LFS) and directly put on the desired device (e.g. using ADB for an Android device, see
[Tutorial 1](tutorials/tutorial_1_simple_cascade.ipynb)). Some models to
consider include:

* [Gemma 2B Instruction Tuned](https://huggingface.co/google/gemma-2b-it)
* [Mistral 7B](https://huggingface.co/TheBloke/Mistral-7B-Instruct-v0.2-GGUF)
* [TinyLlama](https://huggingface.co/TheBloke/TinyLlama-1.1B-Chat-v1.0-GGUF)

When running the model in your GenC code, you'll need to include the absolute
model path, the number of threads LlamaCpp can use, and the max tokens to generate (responses may end early if a EOS is detected). For example, if creating the IR in Python to run on an Android phone the following could be used:

```
my_chain = chains.LLMChain(
      llm=interop.langchain.CustomModel(uri="/device/llamacpp",
      config={"model_path": "/data/local/tmp/gemma-2b-it-q4_k_m.gguf",
            "num_threads": 4,
            "max_tokens": 64}),
      prompt=prompts.PromptTemplate(
          input_variables=["question"],
          template="Q: {question}? A: ",
      ),
  )
```

