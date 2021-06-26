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

#ifndef LIBS_BNG_BNG_DATA_H_
#define LIBS_BNG_BNG_DATA_H_

#include "bng/bng_defines.h"
#include "bng/elem_mol_type.h"
#include "bng/rxn_rule.h"
#include "bng/cplx.h"

namespace BNG {

class BNGData;

class SeedSpecies {
public:
  SeedSpecies(const BNGData* bng_data)
    : cplx(bng_data),
      count(0) {
  }

  Cplx cplx; // may also contain compartment where molecules should be released
  double count; // count of molecules to be released (exact value from the BNGL file)
};


enum class ObservableType {
  Invalid,
  Species,
  Molecules
};


class Observable {
public:
  Observable()
    : type(ObservableType::Invalid) {
  }

  ObservableType type;
  std::string name;
  std::vector<Cplx> patterns;
};


class Compartment {
public:
  Compartment()
    : id(COMPARTMENT_ID_INVALID), is_3d(true), volume_or_area(FLT_INVALID),
      parent_compartment_id(COMPARTMENT_ID_INVALID)
      {
  }
  compartment_id_t id;
  std::string name;
  bool is_3d; // 2d if this member is false

private:
  // - if is_3d is true then this is volume in um^3
  // - if is_3d is false then this is area in um^2 and volume can be obtained by multiplying it
  //     with SURFACE_COMPARTMENT_THICKNESS
  double volume_or_area;

public:
  compartment_id_t parent_compartment_id; // COMPARTMENT_ID_INVALID if the compartment has no parents
  CompartmentIdSet children_compartments; // those are direct children

  bool is_volume_or_area_set() const {
    return volume_or_area != FLT_INVALID;
  }

  double get_volume() const {
    assert(is_3d);
    return volume_or_area;
  }

  double get_area() const {
    assert(!is_3d);
    return volume_or_area;
  }

  void set_volume(const double volume) {
    assert(is_3d);
    volume_or_area = volume;
  }

  void set_area(const double area) {
    assert(!is_3d);
    volume_or_area = area;
  }

  bool has_parent() const {
    return parent_compartment_id != COMPARTMENT_ID_INVALID;
  }

  bool has_children() const {
    return !children_compartments.empty();
  }

  // asserts when volume or area was not set
  double get_volume_including_children(
      const BNGData& bng_data, const bool count_surface_compartments = true) const;
};


/**
 * Data shared among all instances of BNGEngines
 * Usually constant, initialized when BNGL is parsed
 */
class BNGData {
private:
  // indexed with state_id_t
  std::vector<std::string> state_names;

  // indexed with component_type_id_t,
  // contains unique component types even though their name may be identical
  std::vector<ComponentType> component_types;

  // indexed with molecule_type_id_t
  std::vector<ElemMolType> elem_mol_types;

  // indexed with compartment_id_t
  std::vector<Compartment> compartments;

  // indexed with rxn_rule_id_t
  // rxn rules are then in rxn container, this is a temporary placeholder
  // for parsing result
  // TODO: remove and fix dump, not used?
  std::vector<RxnRule> rxn_rules;
  
  // not referenced by any other data in this class,
  // keeping for cases when the parameter values might be needed for other purposes
  std::map<std::string, double> parameters;

  // contents of the seed species section
  // not used directly but can be converted to other representations
  std::vector<SeedSpecies> seed_species;

  // contents of the observables section
  // not used directly but can be converted to other representations
  std::vector<Observable> observables;

public:
  void clear();

  // -------- component state names --------

  state_id_t find_or_add_state_name(const std::string& s);

  // may return STATE_ID_INVALID when the name was not found
  state_id_t find_state_id(const std::string& name) const;

  const std::string& get_state_name(const state_id_t id) const {
    assert(id < state_names.size());
    return state_names[id];
  }


  // -------- components --------

  // - returns COMPONENT_TYPE_ID_INVALID when merge_allowed_states is false and
  //   a component with the same name exists but its allowed states are not a subset
  // - when merge_allowed_states is true, checks only name and merges allowed states
  component_type_id_t find_or_add_component_type(
      const ComponentType& ct, const bool merge_allowed_states = false);

  // may return COMPONENT_TYPE_ID_INVALID when the name was not found
  // among components allowed for this molecule type
  component_type_id_t find_component_type_id(const ElemMolType& mt, const std::string& name) const;

  const ComponentType& get_component_type(const component_type_id_t id) const {
    assert(id < component_types.size());
    return component_types[id];
  }

  ComponentType& get_component_type(const component_type_id_t id) {
    assert(id < component_types.size());
    return component_types[id];
  }

  // -------- molecule types --------

  elem_mol_type_id_t find_or_add_elem_mol_type(const ElemMolType& mt);

  // may return MOLECULE_TYPE_ID_INVALID when the name was not found
  elem_mol_type_id_t find_elem_mol_type_id(const std::string& name) const;

  const ElemMolType& get_elem_mol_type(const elem_mol_type_id_t id) const {
    assert(id < elem_mol_types.size());
    return elem_mol_types[id];
  }

  ElemMolType& get_elem_mol_type(const elem_mol_type_id_t id) {
    assert(id < elem_mol_types.size());
    return elem_mol_types[id];
  }

  const std::vector<ElemMolType>& get_elem_mol_types() const {
    return elem_mol_types;
  }

  // -------- compartments --------
  // asserts if compartment with the same name already exists
  compartment_id_t add_compartment(const Compartment& c);

  // returns COMPARTMENT_ID_INVALID if compartment with this name was not found
  compartment_id_t find_compartment_id(const std::string& name) const;

  // returns nullptr if compartment with this name was not found
  Compartment* find_compartment(const std::string& name);
  const Compartment* find_compartment(const std::string& name) const;

  Compartment& get_compartment(const compartment_id_t id) {
    assert(id < compartments.size());
    return compartments[id];
  }

  const Compartment& get_compartment(const compartment_id_t id) const {
    assert(id < compartments.size());
    return compartments[id];
  }

  const std::vector<Compartment>& get_compartments() const {
    return compartments;
  }

  // returns compartment ids in such an order where all parents appear first
  // example: EC, PM1, CP1, PM2, CP2  (PM's parent is EC and CP1's parent is PM1)
  void get_compartments_sorted_by_parents_first(
      std::vector<compartment_id_t>& sorted_compartment_ids) const;

  // -------- reaction rules --------

  rxn_rule_id_t find_or_add_rxn_rule(const RxnRule& rr);

  const std::vector<RxnRule>& get_rxn_rules() const {
    return rxn_rules;
  }


  // -------- seed species --------

  void add_seed_species(const SeedSpecies& ss) {
    seed_species.push_back(ss);
  }

  const std::vector<SeedSpecies>& get_seed_species() const {
    return seed_species;
  }

  // -------- observables --------

  void add_observable(const Observable& o) {
    observables.push_back(o);
  }

  const std::vector<Observable>& get_observables() const {
    return observables;
  }

  // -------- parameters --------

  void add_parameter(const std::string& name, const double& value) {
    assert(parameters.count(name) == 0);
    parameters[name] = value;
  }

  // returns true and sets value if found, returns falser otherwise
  bool get_parameter_value(const std::string& name, double& value) const {
    auto it = parameters.find(name);
    if (it == parameters.end()) {
      return false;
    }
    else {
      value = it->second;
      return true;
    }
  }

  const std::map<std::string, double>& get_parameters() const {
    return parameters;
  }

  // -------- utilities --------
  void dump() const;

private:
  void dump_parameters() const;
  void dump_molecule_types() const;
  void dump_compartments() const;
  void dump_seed_species() const;
  void dump_reaction_rules() const;
};

} /* namespace BNG2 */

#endif /* LIBS_BNG_BNG_DATA_H_ */
