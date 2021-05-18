
#include "test_utils.h"
#include "bng/bng.h"

using namespace std;
using namespace BNG;

std::string get_test_bngl_file_name(const char* source_file) {
	// source_file is a full path obtained with __FILE__
	std::string src(source_file);

	size_t pos = src.find_last_of("/\\");
	release_assert(pos != string::npos);

	return src.substr(0, pos+1) + "test.bngl";
}



static void insert_into_remaining_if_not_processed(
    const species_id_t id,
    set<species_id_t>& remaining_species,
    const set<species_id_t>& processed_species) {

  if (processed_species.count(id) == 0) {
    remaining_species.insert(id);
  }
}


static void collect_species_used_in_rxn_class(
    RxnClass* rxn_class, // may be null
    set<species_id_t>& remaining_species,
    const set<species_id_t>& processed_species,
    set<RxnClass*>& all_rxn_classes
) {

  if (rxn_class == nullptr) {
    return;
  }

  // what species are products? must initialize to get this information
  rxn_class->init_rxn_pathways_and_rates();

  for (uint pathway_index = 0; pathway_index < rxn_class->get_num_pathways(); pathway_index++) {

    // collect reactants
    for (species_id_t reactant: rxn_class->reactant_ids) {
      insert_into_remaining_if_not_processed(
          reactant,
          remaining_species, processed_species);
    }

    // collect products, must use get_rxn_products_for_pathway to make sure that
    // the products are initialized
    for (const ProductSpeciesIdWIndices& prod_id_w_index:
        rxn_class->get_rxn_products_for_pathway(pathway_index)) {

      insert_into_remaining_if_not_processed(
          prod_id_w_index.product_species_id,
          remaining_species, processed_species);
    }
  }

  // remember the rxn class
  all_rxn_classes.insert(rxn_class);
}


// returns all reaction classes in the system reachable from seed species
// does not check for too large number of combinations
void generate_network(BNGEngine& bng_engine, set<RxnClass*>& all_rxn_classes) {

  const BNGData& bng_data = bng_engine.get_data();
  const BNGConfig& bng_config = bng_engine.get_config();

  set<species_id_t> remaining_species;
  set<species_id_t> processed_species;

  // initialize set of remaining_species
  for (const SeedSpecies& seed: bng_data.get_seed_species()) {
    // create species from the seed species complex
    Species s(seed.cplx, bng_data, bng_config);

    // get its id
    species_id_t id = bng_engine.get_all_species().find_or_add(s);

    remaining_species.insert(id);
  }

  // process all species that we encounter
  while (!remaining_species.empty()) {

    species_id_t id  = *remaining_species.begin();
    assert(processed_species.count(id) == 0
        && "remaining_species must not contain species that were already processed");
    processed_species.insert(id);

    // zero-th order reactions are not supported yet

    // unimolecular reactions:
    RxnClass* rxn_class = bng_engine.get_all_rxns().get_unimol_rxn_class(id);
    collect_species_used_in_rxn_class(
        rxn_class,
        remaining_species, processed_species,
        all_rxn_classes);

    // bimolecular reactions:
    // get all reaction classes for each reactant (sets of
    BNG::SpeciesRxnClassesMap* rxn_classes_map =
        bng_engine.get_all_rxns().get_bimol_rxns_for_reactant(id, true);

    if (rxn_classes_map != nullptr) {
      // iterate over all potential reactants that can react with our species (id)
      for (auto& species_rxn_class_pair: *rxn_classes_map) {
        collect_species_used_in_rxn_class(
            species_rxn_class_pair.second,
            remaining_species, processed_species,
            all_rxn_classes);
      }
    }

    remaining_species.erase(id);
  }
}

uint get_num_rxns_in_network(const std::set<BNG::RxnClass*>& all_rxn_classes) {
  uint res = 0;
  for (const RxnClass* rxn_class: all_rxn_classes) {
    res += rxn_class->get_num_pathways();
  }
  return res;
}

void dump_network(const set<RxnClass*>& all_rxn_classes) {
  for (const RxnClass* rxn_class: all_rxn_classes) {
    std::cout << rxn_class->to_str("", true); // includes newline(s)
  }
}

