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

#include "genc/cc/modules/retrieval/bi_map.h"

#include <optional>
#include <string>

#include "googletest/include/gtest/gtest.h"

namespace genc {

namespace {

TEST(BiMapTest, InsertSuccessfully) {
  BiMap<int, std::string> int_str_map;
  EXPECT_TRUE(int_str_map.Insert(1, "one"));
  EXPECT_EQ(int_str_map.FindByKey(1).value(), "one");
}

TEST(BiMapTest, InsertFailureOnDuplicateKey) {
  BiMap<int, std::string> int_str_map;
  int_str_map.Insert(1, "one");
  EXPECT_FALSE(int_str_map.Insert(1, "duplicate"));
}

TEST(BiMapTest, InsertFailureOnDuplicateValue) {
  BiMap<int, std::string> int_str_map;
  int_str_map.Insert(1, "one");
  EXPECT_FALSE(int_str_map.Insert(2, "one"));
}

TEST(BiMapTest, UpsertUpdatesExistingValue) {
  BiMap<int, std::string> int_str_map;
  int_str_map.Insert(1, "one");
  EXPECT_TRUE(int_str_map.Upsert(1, "updated_one"));
  // EXPECT_EQ(int_str_map.FindByKey(1).value(), "updated_one");
}

TEST(BiMapTest, UpsertInsertsNewValue) {
  BiMap<int, std::string> int_str_map;
  EXPECT_TRUE(int_str_map.Upsert(1, "one"));
  EXPECT_EQ(int_str_map.FindByKey(1).value(), "one");
}

TEST(BiMapTest, RemoveByKeyRemovesPair) {
  BiMap<int, std::string> int_str_map;
  int_str_map.Insert(1, "one");
  EXPECT_EQ(int_str_map.FindByKey(1).value(), "one");
  EXPECT_TRUE(int_str_map.RemoveByKey(1));
  EXPECT_EQ(int_str_map.FindByKey(1), std::nullopt);
}

TEST(BiMapTest, RemoveByKeyNotFoundReturnsFalse) {
  BiMap<int, std::string> int_str_map;
  EXPECT_FALSE(int_str_map.RemoveByKey(1));
}

TEST(BiMapTest, RemoveByValueRemovesPair) {
  BiMap<int, std::string> int_str_map;
  int_str_map.Insert(1, "one");
  EXPECT_EQ(int_str_map.FindByValue("one").value(), 1);
  EXPECT_TRUE(int_str_map.RemoveByValue("one"));
  EXPECT_EQ(int_str_map.FindByValue("one"), std::nullopt);
}

TEST(BiMapTest, RemoveByValueNotFoundReturnsFalse) {
  BiMap<int, std::string> int_str_map;
  EXPECT_FALSE(int_str_map.RemoveByValue("one"));
}

TEST(BiMapTest, FindByKeyNotFoundReturnsNullopt) {
  BiMap<int, std::string> int_str_map;
  EXPECT_EQ(int_str_map.FindByKey(1), std::nullopt);
}

TEST(BiMapTest, FindByValueNotFoundReturnsNullopt) {
  BiMap<int, std::string> int_str_map;
  EXPECT_EQ(int_str_map.FindByValue("one"), std::nullopt);
}

TEST(BiMapTest, SizeReturnsCorrectValue) {
  BiMap<int, std::string> int_str_map;
  EXPECT_EQ(int_str_map.size(), 0);
  int_str_map.Insert(1, "one");
  EXPECT_EQ(int_str_map.size(), 1);
  int_str_map.Insert(2, "two");
  EXPECT_EQ(int_str_map.size(), 2);
  int_str_map.RemoveByKey(1);
  EXPECT_EQ(int_str_map.size(), 1);
}

}  // namespace
}  // namespace genc
