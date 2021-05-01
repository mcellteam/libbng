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

#ifndef LIBS_BNG_ELEM_MOL_TYPE_H_
#define LIBS_BNG_ELEM_MOL_TYPE_H_

#include <string>

#include "bng/bng_defines.h"
#include "bng/base_flag.h"

namespace BNG {

class BNGData;

class ComponentType {
public:
  // name of the component itself is not sufficient to uniquely identify it,
  // it must be always searched with its elementary molecule type name in context
  std::string name;
  // using elementary molecule type name instead of ID becaue during construction
  std::string elem_mol_type_name;

  uint_set<state_id_t> allowed_state_ids;

  bool operator ==(const ComponentType& ct2) const {
    // ordering of allowed states is not important (they are in a set anyway)
    // two states must have the same id, this is ensured in find_or_add_state_name
    return name == ct2.name && allowed_state_ids == ct2.allowed_state_ids;
  }

  std::string to_str(const BNGData& bng_data) const;
  void dump(const BNGData& bng_data) const;
};


// Information common both to MolType and Species
class ElemMolTypeSpeciesCommonData {
public:
  ElemMolTypeSpeciesCommonData()
    : D(FLT_INVALID),
      time_step(FLT_INVALID), space_step(FLT_INVALID),
      color_set(false), color_r(1), color_g(0), color_b(0), scale(1) {
  }

  ElemMolTypeSpeciesCommonData(const ElemMolTypeSpeciesCommonData& other)
    : D(other.D),
      time_step(other.time_step),
      space_step(other.space_step),
      color_set(other.color_set),
      color_r(other.color_r), color_g(other.color_g), color_b(other.color_b),
      scale(other.scale) {
  }

  double D; // diffusion constant

  // computed values, set differently for ElemMolType and Species
  double time_step;
  double space_step;

  // visualization
  void set_color(double r, double g, double b) {
    color_r = r;
    color_g = g;
    color_b = b;
    color_set = true;
  }
  void set_scale(double s) {
    scale = s;
  }

  bool color_set;
  double color_r, color_g, color_b ;  // mol color default is red
  double scale; // scale = 1 by default

};


// Molecule type determines all allowed components and states of these components.
// It is only used to check that reactions and instantiations (releases) follow the
// allowed components and states.
class ElemMolType: public BaseSpeciesCplxMolFlag, public ElemMolTypeSpeciesCommonData {
public:
  ElemMolType() :
    custom_time_step(0), custom_space_step(0) {
  }

  std::string name;
  std::vector<component_type_id_t> component_type_ids;

  // when the user supplied a custom step, the attribute
  // is set to non-zero value, max one of them can be set to a non-zero value
  double custom_time_step;
  double custom_space_step;

  bool has_custom_time_or_space_step() const {
    return custom_time_step != 0 || custom_space_step != 0;
  }

  void compute_space_and_time_step(const BNGConfig& config);

  bool cant_initiate() const {
    return has_flag(SPECIES_MOL_FLAG_TARGET_ONLY);
  }

  bool operator ==(const ElemMolType& mt2) const {
    // ordering of components in a molecule is important
    // two component types must have the same id, this is ensured in find_or_add_component_type
    // diffusion constant and custom time/space steps are ignored,
    // flags are ignored as well
    return name == mt2.name && component_type_ids == mt2.component_type_ids;
  }

  uint get_component_uses_count(const component_type_id_t counted_ct_id) const {
    uint res = 0;
    for (component_type_id_t ct_id: component_type_ids) {
      if (counted_ct_id == ct_id) {
        res++;
      }
    }
    return res;
  }

  std::string to_str(const BNGData& bng_data) const;
  void dump(const BNGData& bng_data) const;
};


// auxiliary functions used by ElemMolType and Species
double get_default_space_step(const BNGConfig& config, const double D);

void get_space_and_time_step(
    const BNGConfig& config,
    const bool is_surf, const double D,
    const double custom_time_step, const double custom_space_step,
    double& time_step, double& space_step);

} /* namespace BNG */

#endif /* LIBS_BNG_ELEM_MOL_TYPE_H_ */
