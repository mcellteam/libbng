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

#ifndef LIBS_BNG_RXN_COMPARTMENT_UTILS_H_
#define LIBS_BNG_RXN_COMPARTMENT_UTILS_H_


namespace BNG {
class BNGData;
class RxnRule;

// - MCell-specific, replaces compartments (including @IN and @OUT) with appropriate orientations
// - keeps the original compartment information there but it is ignored
//   (considered to be COMPARTMENT_ID_NONE)
// - the conversion is equivalent as long as there are no reactions that change the MCell
//   orientation of surface molecules, the assumption is that all surface molecules are oriented
//   outwards (up) all the time
// - returns nonempty string if there was and error
std::string process_compartments_and_set_orientations(const BNGData& bng_data, RxnRule& r);

}

#endif /* LIBS_BNG_RXN_COMPARTMENT_UTILS_H_ */
