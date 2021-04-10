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

#ifndef LIBS_BNG_BNGL_NAMES_H_
#define LIBS_BNG_BNGL_NAMES_H_

namespace BNG {

const char* const BEGIN_PARAMETERS = "begin parameters";
const char* const END_PARAMETERS = "end parameters";

const char* const BEGIN_MOLECULE_TYPES = "begin molecule types";
const char* const END_MOLECULE_TYPES = "end molecule types";

const char* const BEGIN_COMPARTMENTS = "begin compartments";
const char* const END_COMPARTMENTS = "end compartments";

const char* const BEGIN_SEED_SPECIES = "begin seed species";
const char* const END_SEED_SPECIES = "end seed species";

const char* const BEGIN_OBSERVABLES = "begin observables";
const char* const END_OBSERVABLES = "end observables";

const char* const BEGIN_REACTION_RULES = "begin reaction rules";
const char* const END_REACTION_RULES = "end reaction rules";

const char* const OBSERVABLE_SPECIES = "Species";
const char* const OBSERVABLE_MOLECULES = "Molecules";

// these variants are accepted by BNGL2.pl
const char* const COMPLEX_Null = "Null";
const char* const COMPLEX_NULL = "NULL";
const char* const COMPLEX_null = "null";

const char* const COMPLEX_Trash = "Trash";
const char* const COMPLEX_TRASH = "TRASH";
const char* const COMPLEX_trash = "trash";

const char* const ITERATIONS = "ITERATIONS";

const char* const MCELL_TIME_STEP = "MCELL_TIME_STEP";

const char* const MCELL_DIFFUSION_CONSTANT_3D_PREFIX = "MCELL_DIFFUSION_CONSTANT_3D_";
const char* const MCELL_DIFFUSION_CONSTANT_2D_PREFIX = "MCELL_DIFFUSION_CONSTANT_2D_";
const char* const MCELL_REDEFINE_PREFIX = "MCELL_REDEFINE_";

const char* const PARAM_VOL_RXN = "VOL_RXN";
const char* const NA_VALUE_STR = "6.02214e23";
const char* const PARAM_NA_V = "NA_V";
const char* const PARAM_V = "V";

const char* const IND = "  ";

// reserved name of a default compartment that is not printed when a complex's name is printed
const char* const DEFAULT_COMPARTMENT_NAME = "default_compartment";

// special BNGL parameter name used to specify defualt compartment volume
const char* const MCELL_DEFAULT_COMPARTMENT_VOLUME = "MCELL_DEFAULT_COMPARTMENT_VOLUME";

} // namespace BNG

#endif // LIBS_BNG_BNGL_NAMES_H_
