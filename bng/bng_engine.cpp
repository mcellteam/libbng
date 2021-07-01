/******************************************************************************
 * Copyright (C) 2020-2021 by
 * The Salk Institute for Biological Studies
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
******************************************************************************/

#include <iostream>
#include <sstream>

#include "bng/bng_engine.h"
#include "bng/bngl_names.h"

#define BOOST_POOL_NO_MT
#include <boost/pool/pool_alloc.hpp>
#undef BOOST_POOL_NO_MT

using namespace std;

namespace BNG {


void BNGEngine::initialize() {
  // insert information on rxn rules into rxn container
  for (const RxnRule& r: data.get_rxn_rules()) {
    all_rxns.add_and_finalize(r);
  }
}

string BNGEngine::get_stats_report() const {
  stringstream res;

  std::set<reactant_class_id_t> active_reactant_classes;
  uint num_active_species = 0;
  for (const Species* s: all_species.get_species_vector()) {
    release_assert(s != nullptr);
    if (s->was_instantiated()) {
      num_active_species++;
      if (s->has_valid_reactant_class_id()) {
        active_reactant_classes.insert(s->get_reactant_class_id());
      }
    }
  }

  res << "[" <<
      "active/total species " << num_active_species << "/" << all_species.get_species_vector().size() <<
      ", rxn classes " << all_rxns.get_num_rxn_classes() <<
      ", active/total reactant classes " << active_reactant_classes.size() << "/" << all_rxns.get_num_existing_reactant_classes() <<
      "]";
  return res.str();
}


Cplx BNGEngine::create_cplx_from_species(
    const species_id_t id, const orientation_t o, const compartment_id_t compartment_id) const {
  const Cplx& ref = all_species.get(id);
  Cplx copy = ref;
  copy.set_orientation(o);
  copy.set_compartment_id(compartment_id);
  return copy;
}



std::string BNGEngine::export_to_bngl(
    std::ostream& out_parameters,
    std::ostream& out_molecule_types,
    std::ostream& out_compartments,
    std::ostream& out_reaction_rules,
    const bool rates_for_nfsim,
    const double volume_um3_for_nfsim,
    const double area_um3_for_nfsim) const {

  string err_msg;

  export_molecule_types_as_bngl(out_parameters, out_molecule_types);

  err_msg += export_reaction_rules_as_bngl(
      out_parameters, out_reaction_rules, rates_for_nfsim, volume_um3_for_nfsim, area_um3_for_nfsim);

  err_msg += export_compartments_as_bngl(out_parameters, out_compartments);

  return err_msg;
}


void BNGEngine::export_molecule_types_as_bngl(std::ostream& out_parameters, std::ostream& out_molecule_types) const {
  out_molecule_types << BEGIN_MOLECULE_TYPES << "\n";

  out_parameters << "\n" << BNG::IND << "# diffusion constants\n";
  for (const ElemMolType& mt: data.get_elem_mol_types()) {
    if (mt.is_reactive_surface() || is_species_superclass(mt.name)) {
      continue;
    }

    // define as mol type
    out_molecule_types << IND << mt.to_str(data) << "\n";

    // and also set its diffusion constant as parameter
    if (mt.is_vol()) {
      out_parameters << IND << MCELL_DIFFUSION_CONSTANT_3D_PREFIX;
    }
    else if (mt.is_surf()){
      out_parameters << IND << MCELL_DIFFUSION_CONSTANT_2D_PREFIX;
    }
    out_parameters << mt.name << " " << f_to_str(mt.D) << "\n";
  }

  out_molecule_types << END_MOLECULE_TYPES << "\n";
}


static void generate_rxn_rate_conversion_factors(
    std::ostream& out_parameters,
    const bool rates_for_nfsim,
    const double volume_um3_for_nfsim,
    const double area_um3_for_nfsim) {

  out_parameters << "\n";
  out_parameters << IND << PARAM_THICKNESS << " 0.01 # um, assumed membrane thickness\n";
  if (rates_for_nfsim) {
    out_parameters << IND << "# volume rxn rate conversion factor for NFSim\n";
    out_parameters << IND << PARAM_RATE_CONV_VOLUME << " " <<
        f_to_str(volume_um3_for_nfsim) << " * 1e-15\n";
    out_parameters << "\n";
    out_parameters << IND << "# surface-surface rxn rate conversion factor for NFSim\n";
    out_parameters << IND << PARAM_RATE_CONV_THICKNESS <<
        " 1/" <<  f_to_str(area_um3_for_nfsim) << "\n";
  }
  else {
    out_parameters << IND << "# volume rxn rate conversion factor for um^3 to litres\n";
    out_parameters << IND << PARAM_RATE_CONV_VOLUME << " 1e-15\n";
    out_parameters << "\n";
    out_parameters << IND << "# surface-surface rxn rate conversion factor for um^2 to um^3 using membrane thickness, in um\n";
    out_parameters << IND << PARAM_RATE_CONV_THICKNESS << " " << PARAM_THICKNESS << "\n";
  }


  out_parameters << "\n" << BNG::IND << "# parameters to convert rates in MCell and BioNetGen\n";

  out_parameters << IND << PARAM_MCELL2BNG_VOL_CONV << " " << NA_VALUE_STR << " * " << PARAM_RATE_CONV_VOLUME << "\n";
  out_parameters << IND << PARAM_VOL_RXN << " 1\n";
  out_parameters << IND << MCELL_REDEFINE_PREFIX << PARAM_VOL_RXN << " " << PARAM_MCELL2BNG_VOL_CONV << "\n\n";

  out_parameters << IND << PARAM_MCELL2BNG_SURF_CONV << " 1/" << PARAM_RATE_CONV_THICKNESS << "\n";
  out_parameters << IND << PARAM_SURF_RXN << " 1\n";
  out_parameters << IND << MCELL_REDEFINE_PREFIX << PARAM_SURF_RXN << " " << PARAM_MCELL2BNG_SURF_CONV << "\n\n";
}


std::string BNGEngine::export_reaction_rules_as_bngl(
    std::ostream& out_parameters,
    std::ostream& out_reaction_rules,
    const bool rates_for_nfsim,
    const double volume_um3_for_nfsim,
    const double area_um3_for_nfsim) const {

  std::string err_msg;

  generate_rxn_rate_conversion_factors(
      out_parameters, rates_for_nfsim, volume_um3_for_nfsim, area_um3_for_nfsim);

  out_reaction_rules << BEGIN_REACTION_RULES << "\n";

  out_parameters << "\n" << BNG::IND << "# reaction rates\n";

  for (size_t i = 0; i < get_all_rxns().get_rxn_rules_vector().size(); i++) {
    const RxnRule* rr = get_all_rxns().get_rxn_rules_vector()[i];

    string rxn_as_bngl = rr->to_str(false, false, false);

    string rate_param = "k" + to_string(i);
    out_parameters << IND << rate_param << " " << f_to_str(rr->base_rate_constant);

    if (rr->is_reactive_surface_rxn()) {
      err_msg += "Cannot express surface class reaction in BNGL, error for " + rxn_as_bngl + ".\n";
      continue;
    }
    else if (rr->is_bimol()) {
      if (rr->is_vol_rxn() || rr->is_bimol_vol_surf_rxn()) {
        // vol-vol and vol-surf rxns in nfsim use volume of the compartment for conversion,
        // ODE and other methods need just conversion from 1/M*1/s -> um^3*1/s
        out_parameters << " * (" << PARAM_VOL_RXN << " / " << PARAM_MCELL2BNG_VOL_CONV << ")";
      }
      else if (rr->is_bimol_surf_surf_rxn()) {
        // NFSim uses volume (area * 10nm)
        // ODE and other methods need just conversion from um^2*1/s -> um^3*1/s (with membrane thickness 10nm)
        out_parameters << " * (" << PARAM_SURF_RXN << " / " << PARAM_MCELL2BNG_SURF_CONV << ")";
      }
      else {
        err_msg += "Internal error, unexpected reaction type for " + rxn_as_bngl + ".\n";
        continue;
      }
    }
    else if (rr->is_unimol()) {
      // ok, no need to do unit conversion, both tools use 1/s
    }
    else {
      err_msg += "Internal error, unexpected reaction type for " + rxn_as_bngl + ".\n";
      continue;
    }
    out_parameters << "\n";


    out_reaction_rules << IND << rxn_as_bngl;
    out_reaction_rules << " " << rate_param << "\n";
  }

  out_reaction_rules << END_REACTION_RULES << "\n";
  return "";
}


std::string BNGEngine::export_compartments_as_bngl(
    std::ostream& out_parameters,
    std::ostream& out_compartments) const {

  out_compartments << BEGIN_COMPARTMENTS << "\n";

  std::vector<compartment_id_t> sorted_compartment_ids;
  data.get_compartments_sorted_by_parents_first(sorted_compartment_ids);

  // BNG requires parents to be defined first
  for (compartment_id_t comp_id: sorted_compartment_ids) {
    const Compartment& comp = data.get_compartment(comp_id);
    if (comp.name == DEFAULT_COMPARTMENT_NAME) {
      // ignored
      continue;
    }

    if (comp.is_3d) {
      string vol_name = PREFIX_VOLUME + comp.name;
      out_parameters << IND << vol_name << " " << f_to_str(comp.get_volume()) << " # um^3\n";
      out_compartments << IND << comp.name << " 3 " << vol_name;
    }
    else {
      string area_name = PREFIX_AREA + comp.name;
      out_parameters << IND << area_name << " " << f_to_str(comp.get_area()) << " # um^2\n";
      out_compartments << IND << comp.name << " 2 " << area_name << " * " << PARAM_THICKNESS;
    }

    if (comp.parent_compartment_id != COMPARTMENT_ID_INVALID) {
      out_compartments << " " << data.get_compartment(comp.parent_compartment_id).name << "\n";
    }
    else {
      out_compartments << "\n";
    }
  }

  out_compartments << END_COMPARTMENTS << "\n";

  return "";
}

} // namespace BNG
