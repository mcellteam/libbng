/******************************************************************************
 * Copyright (C) 2020-2021 by
 * The Salk Institute for Biological Studies
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
******************************************************************************/

#include <iostream>
#include <algorithm>

#include "bng/bng_data.h"
#include "bng/bngl_names.h"

using namespace std;

namespace BNG {


static double get_compartment_volume_recursively(
    const BNGData& bng_data,
    const compartment_id_t id,
    const bool count_surface_compartments) {

  double res = 0;

  // count children
  const Compartment& comp = bng_data.get_compartment(id);
  for (compartment_id_t child_id: comp.children_compartments) {
    res += get_compartment_volume_recursively(bng_data, child_id, count_surface_compartments);
  }

  // and its own volume
  if (comp.is_3d) {
    res += comp.get_volume();
  }
  else {
    if (count_surface_compartments) {
      res += comp.get_area() * SURFACE_COMPARTMENT_THICKNESS;
    }
  }

  return res;
}


double Compartment::get_volume_including_children(
    const BNGData& bng_data, const bool count_surface_compartments) const {

  return get_compartment_volume_recursively(bng_data, id, count_surface_compartments);
}


void BNGData::clear() {
  state_names.clear();
  component_types.clear();
  elem_mol_types.clear();
  rxn_rules.clear();
}


state_id_t BNGData::find_or_add_state_name(const std::string& s) {
  // rather inefficient search but most probably sufficient for now
  for (state_id_t i = 0; i < state_names.size(); i++) {
    if (state_names[i] == s) {
      return i;
    }
  }

  // not found
  state_names.push_back(s);
  return state_names.size() - 1;
}


// may return STATE_ID_INVALID when the name was not found
state_id_t BNGData::find_state_id(const std::string& name) const {
  for (state_id_t i = 0; i < state_names.size(); i++) {
    if (state_names[i] == name) {
      return i;
    }
  }
  return ELEM_MOL_TYPE_ID_INVALID;
}


component_type_id_t BNGData::find_or_add_component_type(
    const ComponentType& ct,
    const bool merge_allowed_states) {
  assert(ct.elem_mol_type_name != "");
  for (component_type_id_t i = 0; i < component_types.size(); i++) {
    if (component_types[i].elem_mol_type_name == ct.elem_mol_type_name &&
        component_types[i].name == ct.name) {

      if (!merge_allowed_states) {
        // check that the allowed_state_ids is equal or a subset
        if (std::includes(
            component_types[i].allowed_state_ids.begin(),
            component_types[i].allowed_state_ids.end(),
            ct.allowed_state_ids.begin(),
            ct.allowed_state_ids.end()
        )) {
          return i;
        }
        else {
          return COMPONENT_TYPE_ID_INVALID;
        }
      }
      else {
        // merge allowed states and return current id
        component_types[i].allowed_state_ids.insert(
            ct.allowed_state_ids.begin(),
            ct.allowed_state_ids.end());
        return i;
      }
    }
  }

  // not found
  component_types.push_back(ct);
  return component_types.size() - 1;
}


component_type_id_t BNGData::find_component_type_id(const ElemMolType& mt, const std::string& name) const {
  for (component_type_id_t ct_id: mt.component_type_ids) {
    if (component_types[ct_id].name == name) {
      return ct_id;
    }
  }
  return COMPONENT_TYPE_ID_INVALID;
}


// compares only name
elem_mol_type_id_t BNGData::find_or_add_elem_mol_type(const ElemMolType& mt) {
  // TODO LATER: check that if there is a molecule type with the same name,
  // it a subset of the same components and allowed states

  elem_mol_type_id_t existing_mt_id = find_elem_mol_type_id(mt.name);
  if (existing_mt_id != ELEM_MOL_TYPE_ID_INVALID) {
    return existing_mt_id;
  }

  // not found
  elem_mol_types.push_back(mt);
  elem_mol_types.back().set_finalized();
  return elem_mol_types.size() - 1;
}


// may return MOLECULE_TYPE_ID_INVALID when the name was not found
elem_mol_type_id_t BNGData::find_elem_mol_type_id(const std::string& name) const {
  for (elem_mol_type_id_t i = 0; i < elem_mol_types.size(); i++) {
    const ElemMolType& mt = elem_mol_types[i];
    if (mt.name == name) {
      return i;
    }
  }
  return ELEM_MOL_TYPE_ID_INVALID;
}


// asserts if compartment with the same name already exists
compartment_id_t BNGData::add_compartment(const Compartment& c) {
  // compartment must not exist and neither be @IN or @OUT
  assert(find_compartment_id(c.name) == COMPARTMENT_ID_INVALID);

  compartment_id_t id = compartments.size();
  compartments.push_back(c);
  compartments.back().id = id;
  return id;
}


// returns COMPARTMENT_ID_INVALID if compartment with this name was not found
compartment_id_t BNGData::find_compartment_id(const std::string& name) const {
  compartment_id_t in_out_id = get_in_or_out_compartment_id(name);
  if (in_out_id != COMPARTMENT_ID_INVALID) {
    return in_out_id;
  }

  for (compartment_id_t i = 0; i < compartments.size(); i++) {
    if (compartments[i].name == name) {
      return i;
    }
  }
  return COMPARTMENT_ID_INVALID;
}


Compartment* BNGData::find_compartment(const std::string& name) {
  compartment_id_t id = find_compartment_id(name);
  if (id == COMPARTMENT_ID_INVALID || is_in_out_compartment_id(id)) {
    return nullptr;
  }
  else {
    return &get_compartment(id);
  }
}

const Compartment* BNGData::find_compartment(const std::string& name) const {
  compartment_id_t id = find_compartment_id(name);
  if (id == COMPARTMENT_ID_INVALID) {
    return nullptr;
  }
  else {
    return &get_compartment(id);
  }
}


static void collect_compartment_children_recursively(
    const BNGData& data,
    const compartment_id_t id,
    set<compartment_id_t>& used_compartment_ids,
    vector<compartment_id_t>& sorted_compartment_ids
) {
  if (used_compartment_ids.count(id) != 0) {
    return;
  }
  used_compartment_ids.insert(id);
  sorted_compartment_ids.push_back(id);

  const Compartment& comp = data.get_compartment(id);
  for (compartment_id_t child_id: comp.children_compartments) {
    collect_compartment_children_recursively(
        data, child_id, used_compartment_ids, sorted_compartment_ids);
  }
}


void BNGData::get_compartments_sorted_by_parents_first(
    std::vector<compartment_id_t>& sorted_compartment_ids) const {

  sorted_compartment_ids.clear();

  // sort by dependencies
  set<compartment_id_t> used_compartment_ids;
  // for each compartment without dependencies
  for (const Compartment& comp: get_compartments()) {
    if (comp.parent_compartment_id == COMPARTMENT_ID_INVALID) {
      collect_compartment_children_recursively(
          *this, comp.id, used_compartment_ids, sorted_compartment_ids
      );
    }
  }
  assert(sorted_compartment_ids.size() == used_compartment_ids.size());
  assert(sorted_compartment_ids.size() == get_compartments().size());
}


rxn_rule_id_t BNGData::find_or_add_rxn_rule(const RxnRule& rr) {
  // TODO LATER: check that if there is a reaction with the same
  //       reactants and products that the reaction rate is the same
  for (rxn_rule_id_t i = 0; i < rxn_rules.size(); i++) {
    if (rxn_rules[i] == rr) {
      return i;
    }
  }

  // not found
  rxn_rule_id_t id = rxn_rules.size();
  rxn_rules.push_back(rr);
  rxn_rules.back().id = id;
  return id;
}


void BNGData::dump_parameters() const {
  cout << BEGIN_PARAMETERS << "\n";

  for (const auto& param_value_pair: parameters) {
    cout << IND << param_value_pair.first << " " << param_value_pair.second << "\n";
  }

  cout << END_PARAMETERS << "\n";
}


void BNGData::dump_molecule_types() const {
  cout << BEGIN_MOLECULE_TYPES << "\n";

  for (const ElemMolType& mt: elem_mol_types) {
    cout << IND;
    mt.dump(*this);
    cout << "\n";
  }

  cout << END_MOLECULE_TYPES << "\n";
}


void BNGData::dump_seed_species() const {
  cout <<  BEGIN_SEED_SPECIES << "\n";

  for (const SeedSpecies& ss: seed_species) {
    cout <<
        IND << ss.cplx.to_str(true) << " " << ss.count << "\n";
  }

  cout << END_SEED_SPECIES << "\n";
}


void BNGData::dump_compartments() const {
  cout <<  BEGIN_COMPARTMENTS << "\n";

  for (const Compartment& c: compartments) {
    cout << IND << c.name << " ";
    if (c.is_3d) {
      cout << 3 << " " << (c.is_volume_or_area_set() ? c.get_volume() : FLT_INVALID);
    }
    else {
      cout << 2 << " " << (c.is_volume_or_area_set() ? c.get_area() : FLT_INVALID)
          << " * " << SURFACE_COMPARTMENT_THICKNESS;
    }

    if (c.parent_compartment_id != COMPARTMENT_ID_INVALID) {
      cout << " " << get_compartment(c.parent_compartment_id).name;
    }
    cout << "\n";
  }

  cout << END_COMPARTMENTS << "\n";
}


void BNGData::dump_reaction_rules() const {
  cout << BEGIN_REACTION_RULES << "\n";

  for (const RxnRule& rr: rxn_rules) {
    cout << IND;
    rr.dump();
    cout << "\n";
  }

  cout << END_REACTION_RULES << "\n";
}


void BNGData::dump() const {

  dump_parameters();
  dump_molecule_types();
  dump_compartments();
  dump_seed_species();
  dump_reaction_rules();
}

} /* namespace BNG2 */
