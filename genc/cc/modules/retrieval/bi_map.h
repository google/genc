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

#ifndef GENC_CC_MODULES_RETRIEVAL_BI_MAP_H_
#define GENC_CC_MODULES_RETRIEVAL_BI_MAP_H_
#include <cstddef>
#include <optional>

#include "absl/base/thread_annotations.h"
#include "absl/container/flat_hash_map.h"
#include "absl/synchronization/mutex.h"

namespace genc {

// Thread-safe bidirectional map.
template <typename K, typename V>
class BiMap {
 public:
  explicit BiMap() = default;
  ~BiMap() = default;

  // Returns true if the insertion is successful.
  bool Insert(const K& key, const V& value) {
    absl::MutexLock lock(&mutex_);
    if (key_to_value_.contains(key) || value_to_key_.contains(value)) {
      return false;
    }
    key_to_value_[key] = value;
    value_to_key_[value] = key;
    return true;
  }

  bool Upsert(const K& key, const V& value) {
    absl::MutexLock lock(&mutex_);

    auto it_key = key_to_value_.find(key);
    auto it_value = value_to_key_.find(value);

    if (it_key != key_to_value_.end() && it_value != value_to_key_.end()) {
      return false;
    }

    if (it_key != key_to_value_.end() && it_key->second != value) {
      value_to_key_.erase(it_key->second);
    }

    if (it_value != value_to_key_.end() && it_value->second != key) {
      key_to_value_.erase(it_value->second);
    }

    key_to_value_[key] = value;
    value_to_key_[value] = key;

    return true;
  }

  // Removes a pair by Key.
  bool RemoveByKey(const K& key) {
    absl::MutexLock lock(&mutex_);
    auto it = key_to_value_.find(key);
    if (it == key_to_value_.end()) {
      return false;
    }

    value_to_key_.erase(it->second);
    key_to_value_.erase(it);
    return true;
  }

  // Removes a pair by Value.
  bool RemoveByValue(const V& value) {
    absl::MutexLock lock(&mutex_);
    auto it = value_to_key_.find(value);
    if (it == value_to_key_.end()) {
      return false;
    }
    key_to_value_.erase(it->second);
    value_to_key_.erase(it);
    return true;
  }

  // Finds the Value by Key.
  std::optional<V> FindByKey(const K& key) const {
    absl::ReaderMutexLock lock(&mutex_);
    auto it = key_to_value_.find(key);
    return it != key_to_value_.end() ? std::optional(it->second) : std::nullopt;
  }

  // Finds Key by Value.
  std::optional<K> FindByValue(const V& value) const {
    absl::ReaderMutexLock lock(&mutex_);
    auto it = value_to_key_.find(value);
    return it != value_to_key_.end() ? std::optional(it->second) : std::nullopt;
  }

  // Returns the size of the map.
  size_t size() const {
    absl::ReaderMutexLock lock(&mutex_);
    return key_to_value_.size();
  }

 private:
  mutable absl::Mutex mutex_;
  absl::flat_hash_map<K, V> key_to_value_ ABSL_GUARDED_BY(mutex_);
  absl::flat_hash_map<V, K> value_to_key_ ABSL_GUARDED_BY(mutex_);
};
}  // namespace genc

#endif  // GENC_CC_MODULES_RETRIEVAL_BI_MAP_H_
