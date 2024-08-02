# Copyright 2023, The GenC Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Tests for to_from_value_proto.py."""

from absl.testing import absltest
from google3.google.protobuf import timestamp_pb2
from genc.proto.v0 import computation_pb2 as pb
from genc.python.base import to_from_value_proto


class ToFromValueProtoTest(absltest.TestCase):

  def _roundtrip_test(self, arg):
    val_pb = to_from_value_proto.to_value_proto(arg)
    self.assertIsInstance(val_pb, pb.Value)
    reconstructed_arg = to_from_value_proto.from_value_proto(val_pb)
    self.assertEqual(arg, reconstructed_arg)

  def test_something(self):
    self._roundtrip_test(10)
    self._roundtrip_test("a")
    self._roundtrip_test(True)
    self._roundtrip_test(0.5)
    self._roundtrip_test(["foo", 99])

    # Test custom_value
    timestamp_message = timestamp_pb2.Timestamp()
    timestamp_message.FromJsonString("1970-01-01T00:00:00Z")
    val_pb = to_from_value_proto.to_value_proto(timestamp_message)
    self.assertIsInstance(val_pb, pb.Value)
    any_val = to_from_value_proto.from_value_proto(val_pb)
    reconstructed_timestamp_message = timestamp_pb2.Timestamp()
    any_val.Unpack(reconstructed_timestamp_message)
    self.assertEqual(timestamp_message, reconstructed_timestamp_message)

  def test_dict_to_value_proto(self):
    arg = {
        "key_1": "value_1",
        "key_2": 1,
        "key_3": 0.5,
        "key_4": True,
        "key_5": ["foo", "bar", 100],
    }
    val_pb = to_from_value_proto.to_value_proto(arg)
    self.assertIsInstance(val_pb, pb.Value)
    reconstructed_arg = {}
    for element in val_pb.struct.element:
      reconstructed_arg[element.label] = to_from_value_proto.from_value_proto(
          element
      )
    self.assertEqual(arg, reconstructed_arg)


if __name__ == "__main__":
  absltest.main()
