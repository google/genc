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

// This file contains the pybind11 definitions for exposing the C++ default
// executor used in demos on the Python side.
//
// General principles:
//   - Python methods defined here (e.g. `.def_*()`) should not contain
//     "business logic". That should be implemented on the underlying C++ class.

#include "genc/cc/examples/executors/executor_stacks.h"
#include "genc/cc/runtime/executor.h"
#include "genc/proto/v0/computation.pb.h"
#include "include/pybind11/cast.h"
#include "include/pybind11/detail/common.h"
#include "include/pybind11/pybind11.h"
#include "include/pybind11/pytypes.h"
#include "include/pybind11/stl.h"
#include "pybind11_abseil/absl_casters.h"
#include "pybind11_abseil/status_casters.h"
#include "pybind11_protobuf/native_proto_caster.h"

namespace genc {

namespace py = ::pybind11;

namespace {

////////////////////////////////////////////////////////////////////////////////
// The Python module definition `executor_bindings`.
//
// This will be used with `import executor_bindings` on the Python side. This
// module should _not_ be directly imported into the public pip API. The methods
// here will raise `NotOkStatus` errors from absl, which are not user friendly.
////////////////////////////////////////////////////////////////////////////////
PYBIND11_MODULE(executor_bindings, m) {
  py::google::ImportStatusModule();

  m.def("create_default_executor", &CreateDefaultExecutor,
        "Creates a default executor with predefined components used in "
        "GenC demos.");
}

}  // namespace
}  // namespace genc
