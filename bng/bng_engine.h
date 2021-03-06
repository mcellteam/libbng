/******************************************************************************
 * Copyright (C) 2020-2021 by
 * The Salk Institute for Biological Studies
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
******************************************************************************/

#ifndef LIBS_BNG_BNG_ENGINE_H_
#define LIBS_BNG_BNG_ENGINE_H_

#include "bng/bng_defines.h"
#include "bng/bng_data.h"
#include "bng/species_container.h"
#include "bng/rxn_container.h"
#include "bng/elem_mol_type.h"
#include "bng/rxn_rule.h"
#include "bng/cplx.h"

namespace BNG {

/**
 * This is the main BNG library class that owns all the data.
 *
 * One option being considered is that for parallel execution,
 * each partition in MCell will have its own BNG engine.
 * During simulation of a single time step, the contents will
 * change e.g. when a new species is created. So it must be
 * possible to synchronize the multiple BNG engines.
 */
class BNGEngine {
private:
  SpeciesContainer all_species;

  RxnContainer all_rxns;

  // data entered by user, reactions reference these data
  BNGData data;

  const BNGConfig& bng_config;

public:
  BNGEngine(const BNGConfig& bng_config_)
    : all_species(data, bng_config_),
      all_rxns(all_species, data, bng_config_), bng_config(bng_config_)
      {
  }

  // is the bnglib is used directly with the output of
  // bng_data, one needs to call initialize to update contents of the RxnContainer
  void initialize();

  std::string get_stats_report() const;

  bool matches_pattern_incl_all_mols_ignore_orientation(
      const Cplx& cplx_pattern,
      const species_id_t species_id
  ) {
    // check all_mols
    const Cplx& cplx_species = all_species.get_as_cplx(species_id);
    if (cplx_pattern.is_simple()) {
      elem_mol_type_id_t eid = cplx_pattern.elem_mols[0].elem_mol_type_id;
      if (eid == all_species.get_all_molecules_elem_mol_type_id()) {
        return true;
      }
      else if (eid == all_species.get_all_volume_molecules_elem_mol_type_id() &&
          cplx_species.is_vol()) {
        return true;
      }
      else if (eid == all_species.get_all_surface_molecules_elem_mol_type_id() &&
          cplx_species.is_surf()) {
        return true;
      }
    }

    return cplx_species.matches_pattern(cplx_pattern, true);
  }

  species_id_t get_rxn_product_species_id(
      const RxnRule* rxn, const uint product_index,
      const species_id_t reactant_a_species_id, const species_id_t reactant_b_species_id
  );

  SpeciesContainer& get_all_species() { return all_species; }
  const SpeciesContainer& get_all_species() const { return all_species; }

  RxnContainer& get_all_rxns() { return all_rxns; }
  const RxnContainer& get_all_rxns() const { return all_rxns; }

  BNGData& get_data() { return data; }
  const BNGData& get_data() const { return data; }

  const BNGConfig& get_config() const { return bng_config; }

  Cplx create_cplx_from_species(const species_id_t id, const orientation_t o, const compartment_id_t compartment_id) const;


  // returns true if conversion was successful
  // only one compartment and volume reactions are supported now
  // returns empty string if everything went well,
  // nonempty string with error message
  std::string export_to_bngl(
      std::ostream& out_parameters,
      std::ostream& out_molecule_types,
      std::ostream& out_compartments,
      std::ostream& out_reaction_rules,
      const bool rates_for_nfsim,
      const double volume_um3_for_nfsim,
      const double area_um3_for_nfsim) const;

  void print_periodic_stats() const {
    std::cout << "BNG report: " << get_stats_report() << "\n";
    all_species.print_periodic_stats();
    all_rxns.print_periodic_stats();
  }

private:

  // when all_mol_types is false, no superclass species neither
  // surface classes are exported
  void export_molecule_types_as_bngl(
      std::ostream& out_parameters, std::ostream& out_molecule_types) const;
  // if error occurred, returns nonempty string with error message
  std::string export_reaction_rules_as_bngl(
      std::ostream& out_parameters,
      std::ostream& out_reaction_rules,
      const bool rates_for_nfsim,
      const double volume_um3_for_nfsim,
      const double area_um3_for_nfsim) const;

  std::string export_compartments_as_bngl(
      std::ostream& out_parameters,
      std::ostream& out_compartments) const;
};



} /* namespace BNG */

#endif /* LIBS_BNG_BNG_ENGINE_H_ */
