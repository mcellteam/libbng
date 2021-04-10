/******************************************************************************
 * Copyright (C) 2020-2021 by
 * The Salk Institute for Biological Studies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
******************************************************************************/

/**
 * Need some file system utility functions because there is no
 * portable alternative that works on all supported systems
 * (except for boost which would need to be compiled).
 * Especially MacOS's std::filesystem is supported from MacOS 10.15 (Catalina).
 * Posix variants don't work with MSVC.
 */

#ifndef LIBS_BNG_FILESYSTEM_UTILS_H_
#define LIBS_BNG_FILESYSTEM_UTILS_H_

#include <string>
#include <vector>

namespace FSUtils {

bool is_dir(const std::string& dir_path);

// exists with error message if the directory could not be created
void make_dir_for_file_w_multiple_attempts(const std::string& file_path);

void make_dir_w_multiple_attempts(const std::string& dir_path);

bool dir_exists(const std::string& dir_path);

void list_dir(const std::string& dir_path, std::vector<std::string>& dirs);

std::string get_current_dir();

} // namespace FSUtils

#endif /* LIBS_BNG_FILESYSTEM_UTILS_H_ */
