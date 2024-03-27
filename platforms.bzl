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

"""Creates 'config_setting' and 'platform' BUILD rules with same constraints."""

def config_setting_and_platform(
        name,
        constraint_values = [],
        visibility = None):
    """Defines 'config_setting', 'platform' rules with same constraints.

    Args:
        name: name of the 'config_setting'. Platform rule's name is suffixed with '_platform'.
        constraint_values: constraints to meet.
        visibility: target visibility.
    """
    native.config_setting(
        name = name,
        constraint_values = constraint_values,
        visibility = visibility,
    )

    native.platform(
        name = name + "_platform",
        constraint_values = constraint_values,
        visibility = visibility,
    )
