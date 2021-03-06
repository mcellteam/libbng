/******************************************************************************
 * Copyright (C) 2020-2021 by
 * The Salk Institute for Biological Studies
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
******************************************************************************/

#include "bng/shared_defines.h"

#ifdef _MSC_VER
// we are not linking boost and this is needed on Windows
#include <exception>
namespace boost {
void __cdecl throw_exception(std::exception& const e) {
    throw e;
}
}
#endif

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

} // namespace BNGCommon

namespace BNG {

char orientation_to_char(const orientation_t o) {
  switch (o) {
    case ORIENTATION_DOWN: return ',';
    case ORIENTATION_NONE: return ';';
    case ORIENTATION_UP: return '\'';
    case ORIENTATION_NOT_SET: return 'u';
    default: return 'x';
  }
}

} // namespace BNG
