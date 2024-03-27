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

// This file contains the pybind11 defintions for exposing the C++ Executor
// interface in Python.
//
// General principles:
//   - Python methods defined here (e.g. `.def_*()`) should not contain
//     "business logic". That should be implemented on the underlying C++ class.
//     The only logic that may exist here is parameter/result conversions (e.g.
//     `OwnedValueId` -> `ValueId`, etc).

#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/executor_stacks.h"
#include "generative_computing/proto/v0/computation.pb.h"
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
// The Python module defintion `executor_bindings`.
//
// This will be used with `import executor_bindings` on the Python side. This
// module should _not_ be directly imported into the public pip API. The methods
// here will raise `NotOkStatus` errors from absl, which are not user friendly.
////////////////////////////////////////////////////////////////////////////////
PYBIND11_MODULE(executor_bindings, m) {
  py::google::ImportStatusModule();

  m.doc() = "Bindings for the C++ ";

  // Provide an `OwnedValueId` class to handle return values from the
  // `Executor` interface.
  //
  // Note: no `init<>()` method defined, this object is only constructor from
  // Executor instances.
  py::class_<OwnedValueId>(m, "OwnedValueId")
      .def_property_readonly("ref", &OwnedValueId::ref)
      .def("__str__",
           [](const OwnedValueId& self) { return absl::StrCat(self.ref()); })
      .def("__repr__", [](const OwnedValueId& self) {
        return absl::StrCat("<OwnedValueId: ", self.ref(), ">");
      });

  // Provide the `Executor` interface.
  //
  // A `dispose` method is purposely not exposed. Though `Executor::Dispose`
  // exists in C++, Python should call `Dispose` via the `OwnedValueId`
  // destructor during garbage collection.
  //
  // Note: no `init<>()` method defined, must be constructed useing the create_*
  // methods defined below.
  py::class_<Executor, std::shared_ptr<Executor>>(m, "Executor")
      .def("create_value", &Executor::CreateValue, py::arg("value_pb"),
           py::return_value_policy::move,
           py::call_guard<py::gil_scoped_release>())
      .def("create_struct", &Executor::CreateStruct,
           py::return_value_policy::move,
           py::call_guard<py::gil_scoped_release>())
      .def("create_selection", &Executor::CreateSelection,
           py::return_value_policy::move,
           py::call_guard<py::gil_scoped_release>())
      .def("create_call", &Executor::CreateCall, py::arg("function"),
           // Allow `argument` to be `None`.
           py::arg("argument").none(true), py::return_value_policy::move,
           py::call_guard<py::gil_scoped_release>())
      .def(
          "materialize",
          [](Executor& e,
             const ValueId& value_id) -> absl::StatusOr<v0::Value> {
            // Construct a new `v0::Value` to write to and return it to Python.
            v0::Value value_pb;
            absl::Status result = e.Materialize(value_id, &value_pb);
            if (!result.ok()) {
              return result;
            }
            return value_pb;
          },
          py::call_guard<py::gil_scoped_release>());

  // Executor construction methods.
  m.def("create_default_local_executor", &CreateDefaultLocalExecutor,
        "Creates a defaul executor with predefined components.");
}

}  // namespace
}  // namespace generative_computing
