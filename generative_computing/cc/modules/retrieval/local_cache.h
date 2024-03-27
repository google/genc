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

#ifndef GENERATIVE_COMPUTING_CC_MODULES_RETRIEVAL_LOCAL_CACHE_H_
#define GENERATIVE_COMPUTING_CC_MODULES_RETRIEVAL_LOCAL_CACHE_H_

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/synchronization/mutex.h"
#include "generative_computing/cc/intrinsics/custom_function.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

constexpr char kLocalCacheReadUri[] = "/local_cache/read";
constexpr char kLocalCacheWriteUri[] = "/local_cache/write";
constexpr char kLocalCacheRemoveUri[] = "/local_cache/remove";

// A thread-safe Local cache that stores vectors of elements per key.
// Convenient for testing and local development, for deployment, please choose a
// more robust distributed cache.
template <typename K, typename V>
class LocalCache {
 public:
  explicit LocalCache(size_t max_messages_per_key)
      : max_messages_per_key_(max_messages_per_key) {}

  // Inserts a keyed message.
  void Put(const K& key, const V& message) {
    absl::WriterMutexLock lock(&mutex_);
    std::vector<V>& messages = cache_[key];

    if (messages.size() >= max_messages_per_key_) {
      messages.erase(messages.begin());
    }

    messages.push_back(message);
  }

  // Retrieves the last n messages associated with a key. Read only.
  // If n is not present, returns everything.
  std::vector<V> Get(const K& key, std::optional<int> n = std::nullopt) const {
    absl::ReaderMutexLock lock(&mutex_);
    const auto& messages = cache_.find(key);

    if (messages == cache_.end()) {
      return {};
    }

    if (n && *n < static_cast<int>(messages->second.size())) {
      return std::vector<V>(messages->second.end() - *n,
                            messages->second.end());
    } else {
      return messages->second;
    }
  }

  // Returns true if there are any messages associated with a key in the cache.
  bool Exists(const K& key) const {
    absl::ReaderMutexLock lock(&mutex_);
    return cache_.find(key) != cache_.end();
  }

  // Returns the number of keys in the cache.
  size_t size() const { return cache_.size(); }

  // Removes all messages associated with a key from the cache.
  void Remove(const K& key) {
    absl::WriterMutexLock lock(&mutex_);
    cache_.erase(key);
  }

 private:
  absl::flat_hash_map<K, std::vector<V>> cache_;
  size_t max_messages_per_key_;
  mutable absl::Mutex mutex_;
};

// Typically used in applications (e.g. chatbot) where access to context is
// required.
template class LocalCache<std::string, std::string>;
using LocalStringCache = LocalCache<std::string, std::string>;
using LocalValueCache = LocalCache<std::string, v0::Value>;

// Make CustomFunctions aware of local cache.
// Doesn't own it, cache must stay alive during the life time of the Runtime.
absl::Status SetCustomFunctionsForLocalValueCache(
    intrinsics::CustomFunction::FunctionMap& fn_map, LocalValueCache& cache,
    std::string default_delimiter = "\n",
    std::string default_key = "default_key");
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_MODULES_RETRIEVAL_LOCAL_CACHE_H_
