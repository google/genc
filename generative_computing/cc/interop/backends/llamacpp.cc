/* Copyright 2024, The Generative Computing Authors.

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
#include "generative_computing/cc/interop/backends/llamacpp.h"

#include <string>
#include <vector>

#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/strings/str_join.h"
#include "generative_computing/cc/intrinsics/model_inference.h"
#include "generative_computing/proto/v0/computation.pb.h"
#include "llama.h"

namespace {
void llama_batch_add(struct llama_batch& batch, llama_token id, llama_pos pos,
                     bool logits) {
  batch.token[batch.n_tokens] = id;
  batch.pos[batch.n_tokens] = pos;
  batch.logits[batch.n_tokens] = logits;

  // Only provide a simple seq_id of {0}
  batch.n_seq_id[batch.n_tokens] = 1;
  batch.seq_id[batch.n_tokens][0] = 0;

  batch.n_tokens++;
}

constexpr int kMaxTokenLength = 32;  // Max token length in characters.
}  // namespace
namespace generative_computing {

absl::Status LlamaCpp::InitModel(absl::string_view model_path,
                                 int num_threads = 1, int max_tokens = 32) {
  llama_backend_init(/*params.numa=*/false);
  llama_model_params model_params = llama_model_default_params();
  model_ =
      llama_load_model_from_file(std::string(model_path).c_str(), model_params);
  if (!model_) {
    return absl::InvalidArgumentError("Unable to create model.");
  }
  llama_context_params ctx_params = llama_context_default_params();
  ctx_params.seed = 1234;
  ctx_params.n_ctx = 1024;
  ctx_params.n_threads = num_threads;
  context_ = llama_new_context_with_model(model_, ctx_params);
  max_tokens_ = max_tokens;
  return absl::OkStatus();
}

absl::StatusOr<v0::Value> LlamaCpp::CreateRequest(std::string prompt) {
  v0::Value request;
  v0::Struct* args = request.mutable_struct_();

  v0::Value* arg_prompt = args->add_element();
  arg_prompt->set_label("prompt");
  arg_prompt->set_str(prompt);
  return request;
}

absl::StatusOr<v0::Value> LlamaCpp::LlamaCppCall(const v0::Value& input) {
  v0::Value response;
  std::string prompt;

  // Unpack the keyword arguments.
  for (const v0::Value& arg : input.struct_().element()) {
    if (arg.label() == "prompt") {
      prompt = arg.str();
    }
  }

  if (!context_ || !model_) {
    return absl::InternalError("LlamaCpp wasn't initialized.");
  }

  LOG(INFO) << "Initial Prompt: " << prompt;

  // Tokenizes prompt.
  int n_tokens = prompt.size() + 1;  // Max token size, plus one for BOS.
  std::vector<llama_token> tokenized_prompt(n_tokens);
  n_tokens = llama_tokenize(model_, prompt.c_str(), prompt.length(),
                            tokenized_prompt.data(), tokenized_prompt.size(),
                            /*add_bos=*/true, /*special=*/true);

  // If a negative token length is returned, resize to a larger container.
  // If not, just resize to the new size.
  if (n_tokens < 0) {
    tokenized_prompt.resize(-n_tokens);
    n_tokens = llama_tokenize(model_, prompt.c_str(), prompt.length(),
                              tokenized_prompt.data(), tokenized_prompt.size(),
                              /*add_bos=*/true, /*special=*/true);
  } else {
    tokenized_prompt.resize(n_tokens);
  }

  const int n_ctx = llama_n_ctx(context_);
  if (tokenized_prompt.size() > n_ctx) {
    return absl::InternalError("Prompt length is too large for context");
  }

  llama_batch batch = llama_batch_init(llama_n_batch(context_), 0, 1);

  // Add initial prompt to batch.
  for (size_t i = 0; i < tokenized_prompt.size(); ++i) {
    llama_batch_add(batch, tokenized_prompt[i], i, false);
  }

  // Set the last token to output logits.
  batch.logits[batch.n_tokens - 1] = true;

  if (llama_decode(context_, batch)) {
    return absl::InternalError("llama_decode() of initial prompt failed");
  }

  // Main prediction loop
  int n_current = batch.n_tokens;
  int n_decoded = 0;
  std::vector<std::string> result;

  const auto start = absl::Now();

  // Allow tokens up to provided length, including prompt.
  // Can still break early on EOS.
  while (n_current <= max_tokens_) {
    int32_t n_vocab = llama_n_vocab(model_);
    float* logits = llama_get_logits_ith(context_, batch.n_tokens - 1);
    std::vector<llama_token_data> candidates(n_vocab);

    for (llama_token id = 0; id < n_vocab; ++id) {
      candidates.emplace_back(llama_token_data{id, logits[id], 0.0f});
    }

    llama_token_data_array candidates_arr = {candidates.data(),
                                             candidates.size(), false};

    const llama_token new_token_id =
        llama_sample_token_greedy(context_, &candidates_arr);
    if (new_token_id == llama_token_eos(model_)) {
      break;
    }

    std::vector<char> piece(kMaxTokenLength);
    const int n_chars =
        llama_token_to_piece(model_, new_token_id, piece.data(), piece.size());
    if (n_chars < 0) {
      piece.resize(-n_chars);
      llama_token_to_piece(
          model_, new_token_id, piece.data(), piece.size());
    } else {
      piece.resize(n_chars);
    }
    result.push_back(std::string(piece.data(), piece.size()));

#if !defined(NDEBUG)
    LOG(INFO) << result.back();
#endif

    // Clear the batch
    batch.n_tokens = 0;

    // Add new token to batch
    llama_batch_add(batch, new_token_id, n_current, true);
    n_decoded++;
    n_current++;

    // Evaluate the current batch
    if (llama_decode(context_, batch)) {
      return absl::InternalError("Failed to eval batch");
    }
  }

  const auto end = absl::Now();
  const std::string result_str = absl::StrJoin(result, "");
  const absl::Duration duration = (end - start);
  LOG(INFO) << result_str;
  LOG(INFO) << "\n\nDecoded " << n_decoded << " tokens in " << duration
            << ", speed: " << n_decoded / absl::ToDoubleSeconds(duration)
            << " t/s";
  response.set_str(result_str);
  return response;
}

absl::Status LlamaCpp::SetInferenceMap(
    intrinsics::ModelInference::InferenceMap& inference_map,
    absl::string_view model_uri) {
  inference_map[model_uri] = [this](v0::Value arg) {
    return LlamaCppCall(arg);
  };
  return absl::OkStatus();
}
}  // namespace generative_computing
