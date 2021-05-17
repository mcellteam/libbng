#include <string>
#include <set>
using namespace std;

#include "bng/bng.h"
#include "../shared/test_utils.h"

using namespace BNG;


void insert_into_remaining_if_not_processed(
    const species_id_t id,
    set<species_id_t>& remaining_species,
    const set<species_id_t>& processed_species) {

  if (processed_species.count(id) == 0) {
    remaining_species.insert(id);
  }
}


void collect_species_used_in_rxn_class(
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



void dump_network(const set<RxnClass*>& all_rxn_classes) {
  for (const RxnClass* rxn_class: all_rxn_classes) {
    std::cout << rxn_class->to_str("", true) << "\n";
  }
}


int main() {

  string file_name = get_test_bngl_file_name(__FILE__);

  BNGConfig bng_config;
  BNGEngine bng_engine(bng_config);
  BNGData& bng_data = bng_engine.get_data();

  // load the test BNG file
  int num_errors = parse_bngl_file(file_name, bng_data);
  release_assert(num_errors == 0);

  // we must initialize the bng_engine now
  bng_engine.initialize();

  set<RxnClass*> all_rxn_classes;
  generate_network(bng_engine, all_rxn_classes);

  // print the result
  dump_network(all_rxn_classes);

  // check the number of rxn classes we got
  assert(all_rxn_classes.size() == 6);
}
