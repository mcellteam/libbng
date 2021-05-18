#include <string>
#include <set>
using namespace std;

#include "bng/bng.h"
#include "../shared/test_utils.h"

using namespace BNG;


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
  //assert(all_rxn_classes.size() == 6);
}
