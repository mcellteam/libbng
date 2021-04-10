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

#ifndef LIBS_BNG_PARSER_UTILS_H_
#define LIBS_BNG_PARSER_UTILS_H_

#include <ostream>

#ifdef _MSC_VER
#include <io.h>
#endif

namespace BNG {

class ASTBaseNode;

std::ostream& errs_loc();
std::ostream& errs_loc(const ASTBaseNode* loc);

double convert_to_dbl(const char* str);
long long convert_dec_to_llong(const char* str);

// same as strdup, only uses 'new' to allocate memory so that we can
// consistently use 'delete' afterwards to free it
char* strdup_new(const char* str);

} // namespace BNG

#endif // LIBS_BNG_PARSER_UTILS_H_
