#include <string>
using namespace std;

#include "bng/bng.h"
#include "../shared/test_utils.h"

int main() {

  string file_name = get_test_bngl_file_name(__FILE__);

  BNG::BNGConfig bng_config;
  BNG::BNGEngine bng_engine(bng_config);
  BNG::BNGData& bng_data = bng_engine.get_data();

  // load the test BNG file
  int num_errors = BNG::parse_bngl_file(file_name, bng_data);
  release_assert(num_errors == 0);

  // we must initialize the bng_engine now
  bng_engine.initialize();

  // create Species A, the simplest way is to use the BNGL parser for it
  BNG::Species A(bng_engine.get_data());
  num_errors = BNG::parse_single_cplx_string("A", bng_data, A);
  release_assert(num_errors == 0);

  // finalize the species add A to to the species list
  A.finalize_species(bng_config, false);
  BNG::species_id_t A_id = bng_engine.get_all_species().find_or_add(A);

  // get unimol rxn class, it must exist
  BNG::RxnClass* rxn_class = bng_engine.get_all_rxns().get_unimol_rxn_class(A_id);
  release_assert(rxn_class != nullptr);

  // the products might not have been computed, make sure that they are
  rxn_class->init_rxn_pathways_and_rates();

  // check that we got back B as the only possible product
  release_assert(rxn_class->pathways.size() == 1);

  const BNG::RxnClassPathway& pathway = rxn_class->pathways[0];
  release_assert(pathway.product_species_w_indices.size() == 1);

  BNG::species_id_t prod_id = pathway.product_species_w_indices[0].product_species_id;

  const BNG::Species& product = bng_engine.get_all_species().get(prod_id);

  release_assert(product.name == "B");
}
