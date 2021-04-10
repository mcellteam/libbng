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

#include <cstdlib>
#include <cerrno>
#include <string>


#include "bng/parser_utils.h"
#include "bngl_parser.hpp"

using namespace std;

extern int bngllineno;
extern BNG::ParserContext* g_ctx;

namespace BNG {

ostream& errs_loc() {
  assert(g_ctx != nullptr);
  cerr <<
      g_ctx->get_current_file_name() << ":" << bngllineno <<
      ": error: ";
  return cerr;
}


ostream& errs_loc(const ASTBaseNode* loc) {
  assert(loc != nullptr);
  assert(loc->has_loc);
  assert(loc->file != nullptr);
  cerr <<
      loc->file << ":" << loc->line <<
      ": error: ";
  return cerr;
}


double convert_to_dbl(const char* str) {
  char* end;
  double res;

  errno = 0; // note: errno is thread-local
  res = strtod(str, &end);
  if (errno != 0 || *end != '\0') {
    errs_loc() << "Could not convert floating point value '" << str << "'.\n";
  }

  return res;
}


long long convert_dec_to_llong(const char* str) {
  char* end;
  long long int res;

  errno = 0; // note: errno is thread-local
  res = strtoll(str, &end, 10);
  if (errno != 0 || *end != '\0') {
    errs_loc() << "Could not convert integer value '" << str << "'.\n";
  }

  return res;
}


char *strdup_new(const char *src)
{
  char *str;
  size_t size = strlen(src) + 1;

  str = new char[size];
  if (str != nullptr) {
    memcpy(str, src, size);
  }
  return str;
}

} // namespace BNG
