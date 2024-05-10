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

#include <jni.h>
#include <memory>
#include <string>
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "genc/cc/interop/networking/cronet_based_android_http_client.h"
#include "genc/cc/interop/networking/http_client_interface.h"
#include "genc/java/src/jni/jni_utils.h"

namespace genc {
namespace interop {
namespace networking {
namespace {

constexpr char kGetFromUrlMethodName[] = "getFromUrl";
constexpr char kGetFromUrlMethodSignature[] = "([B)Ljava/lang/String;";

class CronetBasedAndroidHttpClient : public HttpClientInterface {
 public:
  CronetBasedAndroidHttpClient(
      JavaVM* jvm, jobject simple_synchronous_http_client_interface)
      : jvm_(jvm), simple_synchronous_http_client_interface_(
        simple_synchronous_http_client_interface) {}

  ~CronetBasedAndroidHttpClient() override {}

  absl::StatusOr<std::string> GetFromUrl(
      const std::string& url) override {
    if (jvm_ == nullptr) {
      return absl::InternalError("JVM is null");
    }
    JNIEnv* const env = GetJniEnv(jvm_);
    if (env == nullptr) {
      return absl::InternalError("Couldn't get JNI env");
    }
    jclass java_client_class =
        env->GetObjectClass(simple_synchronous_http_client_interface_);
    if (java_client_class == nullptr) {
      return absl::InternalError(
          "Couldn't retrieve simple synchronous HTTP client class from JNI.");
    }
    jmethodID callMethodId = env->GetMethodID(
        java_client_class, kGetFromUrlMethodName, kGetFromUrlMethodSignature);
    if (callMethodId == 0) {
      return absl::InternalError(absl::StrCat(
          "Couldn't retrieve method \"",
          kGetFromUrlMethodName,
          "\" from simple synchronous HTTP client's class."));
    }
    jbyteArray jrequest = GetJbyteArrayFromString(env, url);
    jobject jresponse = env->CallObjectMethod(
        simple_synchronous_http_client_interface_, callMethodId, jrequest);
    std::string response = GetString(env, (jstring) jresponse);
    env->DeleteLocalRef(jresponse);
    env->DeleteLocalRef(jrequest);
    jvm_->DetachCurrentThread();
    return response;
  }

  absl::StatusOr<std::string> PostJsonToUrl(
      const std::string& url,
      const std::string& json_request) override {
    return absl::UnimplementedError(
        "PostJsonToUrl is not implemented.");
  }

  absl::StatusOr<std::string> GetFromUrlAndSocket(
      const std::string& url,
      const std::string& socket_path) override {
    return absl::UnimplementedError(
        "GetFromUrlAndSocket is not implemented.");
  }

  absl::StatusOr<std::string> PostJsonToUrlAndSocket(
      const std::string& url,
      const std::string& socket_path,
      const std::string& json_request) override {
    return absl::UnimplementedError(
        "PostJsonToUrlAndSocket is not implemented.");
  }

 private:
  JavaVM* const jvm_;
  jobject simple_synchronous_http_client_interface_;
};

}  // namespace

absl::StatusOr<std::shared_ptr<HttpClientInterface>>
CreateCronetBasedAndroidHttpClient(
    JavaVM* jvm, jobject simple_synchronous_http_client_interface) {
  return std::make_shared<CronetBasedAndroidHttpClient>(
      jvm, simple_synchronous_http_client_interface);
}

}  // namespace networking
}  // namespace interop
}  // namespace genc
