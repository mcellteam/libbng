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

#ifndef LIBS_BNG_PARSER_H_
#define LIBS_BNG_PARSER_H_

#include <string>
#include <map>

#include "bng_defines.h"

namespace BNG {

class BNGData;
class Cplx;

// parses input BNGL file and performs semantic analysis
// returns number of errors encountered while parsing
// prints errors and warnings directly to the error output
// !! function is not reentrant
int parse_bngl_file(
    const std::string& file_name,
    BNGData& bng_data,
    const std::map<std::string, double>& parameter_overrides = std::map<std::string, double>()
);


// parses input BNGL complex instance or pattern string and performs
// semantic analysis
// returns number of errors encountered while parsing
// bng_data are not cleared and one can continue with adding
// complexes gradually
// prints errors and warnings directly to the error output
// resulting complex is stored into the res_cplx
int parse_single_cplx_string(
    const std::string& cplx_string, BNGData& bng_data,
    Cplx& res_cplx
);


} /* namespace BNG */

#endif /* LIBS_BNG_PARSER_H_ */
