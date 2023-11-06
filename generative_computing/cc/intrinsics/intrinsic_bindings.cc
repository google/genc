/* Copyright 2023, The Generative Computing Authors.

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

// This file contains the pybind11 definitions for exposing the C++ intrinsic
// bindings and associated APIs in Python.
//
// General principles:
//   - Python methods defined here (e.g. `.def_*()`) should not contain
//     "business logic". That should be implemented on the underlying C++ class.

#include <string>

#include "generative_computing/cc/intrinsics/intrinsic_uris.h"
#include "include/pybind11/cast.h"
#include "include/pybind11/detail/common.h"
#include "include/pybind11/pybind11.h"
#include "include/pybind11/pytypes.h"
#include "include/pybind11/stl.h"
#include "pybind11_abseil/absl_casters.h"
#include "pybind11_abseil/status_casters.h"
#include "pybind11_protobuf/native_proto_caster.h"

namespace generative_computing {

namespace py = ::pybind11;

namespace {

////////////////////////////////////////////////////////////////////////////////
// The Python module definition `intrinsic_bindings`.
//
// This will be used with `import intrinsic_bindings` on the Python side.
////////////////////////////////////////////////////////////////////////////////
PYBIND11_MODULE(intrinsic_bindings, m) {
  py::google::ImportStatusModule();

  m.doc() = "Bindings for the C++ ";

  // Intrinsics.
  auto intrinsics = m.def_submodule("intrinsics");
  intrinsics.attr("BREAKABLE_CHAIN") = py::str(intrinsics::kBreakableChain);
  intrinsics.attr("CONDITIONAL") = py::str(intrinsics::kConditional);
  intrinsics.attr("FALLBACK") = py::str(intrinsics::kFallback);
  intrinsics.attr("LOOP_CHAIN_COMBO") = py::str(intrinsics::kLoopChainCombo);
  intrinsics.attr("LOGICAL_NOT") = py::str(intrinsics::kLogicalNot);
  intrinsics.attr("MODEL_INFERENCE") = py::str(intrinsics::kModelInference);
  intrinsics.attr("PROMPT_TEMPLATE") = py::str(intrinsics::kPromptTemplate);
  intrinsics.attr("REGEX_PARTIAL_MATCH") =
      py::str(intrinsics::kRegexPartialMatch);
  intrinsics.attr("REPEAT") = py::str(intrinsics::kRepeat);
  intrinsics.attr("WHILE") = py::str(intrinsics::kWhile);
}

}  // namespace
}  // namespace generative_computing
