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

#include <string>
#include <sstream>

#include "base_flag.h"

namespace BNG {

#define FLAG_TO_STR(mask) if (((get_flags()) & (mask)) != 0) res << std::string(#mask) + ", ";

std::string BaseSpeciesCplxMolFlag::to_str() const {
  std::stringstream res;

  FLAG_TO_STR(SPECIES_CPLX_MOL_FLAG_SURF)
  FLAG_TO_STR(SPECIES_CPLX_MOL_FLAG_REACTIVE_SURFACE)
  FLAG_TO_STR(SPECIES_CPLX_FLAG_ONE_MOL_NO_COMPONENTS)
  FLAG_TO_STR(SPECIES_FLAG_CAN_VOLVOL)
  FLAG_TO_STR(SPECIES_FLAG_CAN_VOLSURF)
  FLAG_TO_STR(SPECIES_FLAG_CAN_VOLWALL)
  FLAG_TO_STR(SPECIES_FLAG_CAN_SURFSURF)
  FLAG_TO_STR(SPECIES_MOL_FLAG_TARGET_ONLY)
  FLAG_TO_STR(SPECIES_FLAG_CAN_REGION_BORDER)
  FLAG_TO_STR(SPECIES_FLAG_HAS_UNIMOL_RXN)
  FLAG_TO_STR(SPECIES_FLAG_HAS_BIMOL_VOL_RXN)
	
  FLAG_TO_STR(RXN_FLAG_COUNTED_IN_WORLD)
  FLAG_TO_STR(RXN_FLAG_COUNTED_IN_VOLUME_REGIONS)
  FLAG_TO_STR(RXN_FLAG_COUNTED_ON_SURFACE_REGIONS)
  FLAG_TO_STR(RXN_FLAG_SIMPLE)

  res << " (0x" << std::hex << get_flags() << ")";

  return res.str();
}


}
