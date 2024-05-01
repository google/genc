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

#include "googletest/include/gtest/gtest.h"
#include "genc/c/interop/oak/oak_api.h"

namespace genc {
namespace {

TEST(OakApiTest, Something) {
  // TODO(b/333410413): Replace this with a real test that does something.
  OakHpkeKeyPair* x = oak_generate_hpke_key_pair();
  EXPECT_TRUE(x != nullptr);
  oak_delete_hpke_key_pair(x);
}

}  // namespace
}  // namespace genc
