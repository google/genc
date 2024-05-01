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

#![crate_type = "staticlib"]

#[derive(Debug)]
#[repr(C)]
pub struct OakHpkeKeyPair {
    // TODO(b/333410413): Replace this with an instance of the HPKE key pair obtained from Oak.
    boo: i32,
}

#[no_mangle]
pub unsafe extern "C" fn oak_generate_hpke_key_pair() -> *mut OakHpkeKeyPair {
    // TODO(b/333410413): Replace this with code that calls Oak to generate a key
    // pair, probably from `oak_containers_orchestrator/src/crypto.rs`, and
    // return it here packaged as an instance of `OakHpkeKeyPair`.
    let key_pair = OakHpkeKeyPair { boo: 10 };
    return Box::into_raw(Box::new(key_pair));
}

#[no_mangle]
pub extern "C" fn oak_delete_hpke_key_pair(key_pair: *mut OakHpkeKeyPair) {
    let _ = unsafe { Box::from_raw(key_pair) };
}

// TODO(b/333410413): Define the rest of this API.
// - `oak_get_public_key`
// - `oak_create_endorsed_evidence`
// - `oak_decrypt_request`
// - `oak_encrypt_response`
