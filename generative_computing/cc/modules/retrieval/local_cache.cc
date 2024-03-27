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

#include "generative_computing/cc/modules/retrieval/local_cache.h"

#include <sstream>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "generative_computing/cc/intrinsics/custom_function.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
absl::Status SetCustomFunctionsForLocalValueCache(
    intrinsics::CustomFunction::FunctionMap& fn_map, LocalValueCache& cache,
    std::string default_delimiter, std::string default_key) {
  // TODO(b/304905545): default behavior is read all, improve flexibility.
  fn_map[kLocalCacheReadUri] = [&cache, default_delimiter,
                                default_key](const v0::Value& arg) {
    std::string key = default_key;
    std::string delimiter = default_delimiter;

    std::ostringstream str_stream;
    for (const v0::Value& v : cache.Get(key)) {
      str_stream << v.str() << delimiter;
    }

    v0::Value result;
    result.set_str(str_stream.str());
    return result;
  };

  fn_map[kLocalCacheWriteUri] = [&cache, default_key](const v0::Value& arg) {
    // TODO(b/304905545): write to default key. improve flexibility.
    std::string key = default_key;
    cache.Put(key, arg);
    return arg;
  };

  fn_map[kLocalCacheRemoveUri] = [&cache, default_key](const v0::Value& arg) {
    // TODO(b/304905545): write to default key. improve flexibility.
    std::string key = default_key;
    cache.Remove(key);
    return arg;
  };

  return absl::OkStatus();
}
}  // namespace generative_computing
