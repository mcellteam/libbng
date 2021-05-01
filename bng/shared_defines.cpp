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

#include "bng/shared_defines.h"

namespace BNGCommon {

std::string f_to_str(const double val, const int n) {
  std::stringstream out;
  if (val == 0.0) {
    return "0";
  }
  else if (fabs_f(val) <= 0.01 || fabs_f(val) >= 100000) {
    out << std::scientific;
  }
  out.precision(n);
  out << val;
  std::string res = out.str();
  //cut off trailing zeros

  size_t epos = res.find('e');
  if (epos != std::string::npos) {
    int last_nonzero_pos = epos - 1;
    while (res[last_nonzero_pos] == '0' || res[last_nonzero_pos] == '.') {
      last_nonzero_pos--;
      assert(last_nonzero_pos >= 0);
    }
    // cut out the zeros
    if (last_nonzero_pos != (int)epos - 1) {
      res = res.substr(0, last_nonzero_pos+1) + res.substr(epos);
    }
  }

  return res;
}


char orientation_to_char(const orientation_t o) {
  switch (o) {
    case ORIENTATION_DOWN: return ',';
    case ORIENTATION_NONE: return ';';
    case ORIENTATION_UP: return '\'';
    case ORIENTATION_NOT_SET: return 'u';
    default: return 'x';
  }
}

} // namespace BNGCommon
