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

// This file contains the pybind11 definitions for exposing the C++ Constructor
// interface in Python.
//
// General principles:
//   - Python methods defined here (e.g. `.def_*()`) should not contain
//     "business logic". That should be implemented on the underlying C++ class.

#include "generative_computing/cc/authoring/constructor.h"
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
// The Python module definition `constructor_bindings`.
//
// This will be used with `import constructor_bindings` on the Python side. This
// module should _not_ be directly imported into the public pip API. The methods
// here will raise `NotOkStatus` errors from absl, which are not user friendly.
////////////////////////////////////////////////////////////////////////////////
PYBIND11_MODULE(constructor_bindings, m) {
  py::google::ImportStatusModule();

  m.doc() = "Bindings for the C++ Constructors.";

  m.def("create_model", &CreateModelInference,
        "Creates a model computation with the given model URI.");

  m.def(
      "create_prompt_template", &CreatePromptTemplate,
      "Creates a prompt template computation with the given template string.");

  m.def("create_regex_partial_match", &CreateRegexPartialMatch,
        "Creates a regular expression partial match with the given pattern.");

  m.def("create_reference", &CreateReference,
        "Constructs a reference to `name`.");

  m.def(
      "create_lambda", &CreateLambda,
      "Constructs a lambda expression with parameter `name` and body `body`.");

  m.def("create_repeat", &CreateRepeat,
        "Constructs a for loop, that repeats the fn_body n times.");

  m.def("create_call", &CreateCall, "Constructs a function call.");

  m.def("create_serial_chain", &CreateSerialChain,
        "Given a list of functions [f, g, ...] create a chain g(f(...)).");

  m.def("create_while", &CreateWhile,
        "Constructs a function call. condition_fn is evaluated before body_fn "
        "is called each iteration.");

  m.def("create_custom_function", &CreateCustomFunction,
        "Returns a custom function computation with the given user defined "
        "function uri.");

  m.def("create_logical_not", &CreateLogicalNot,
        "Creates a logical negation computation.");

  m.def("create_parallel_map", &CreateParallelMap,
        "Creates a parallel map that applies map_fn to a all input values.");

  m.def(
      "create_repeated_conditional_chain", &CreateRepeatedConditionalChain,
      "Creates a chain that can repeat and break which is a typical construct "
      "for llm reasoning loop.");

  m.def("create_logger", &CreateLogger,
        "Creates a Logger, it takes an input logs it and returns the original "
        "input.");

  m.def("create_breakable_chain", &CreateBreakableChain,
        "Given a list of functions [f, g, ...] create a chain g(f(...)). "
        "Compared to CreateBasicChain, this chain can contain break point as "
        "part of the chain.");

  m.def("create_struct", &CreateStruct,
        "Constructs a struct from named values");

  m.def(
      "create_selection", &CreateSelection,
      "Constructs a selection to pick the i-th element from a source.struct_.");

  m.def("create_conditional", &CreateConditional,
        "Constructs a computation representing if/esle.");

  m.def("create_fallback", &CreateFallback,
        "Constructs a computation a fallback chain.");

  m.def("create_inja_template", &CreateInjaTemplate,
        "Returns an inja template that encodes a template with input JSON "
        "string.");

  m.def("create_rest_call", &CreateRestCall,
        "Returns an operator that makes rest calls");

  m.def("create_wolfram_alpha", &CreateWolframAlpha,
        "Returns an operator that makes calls to WolframAlpha ShortAnswer API");
}

}  // namespace
}  // namespace generative_computing
