#include <string>

#include "bng/bng.h"
#include "../shared/test_utils.h"

using namespace std;
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

  // get the graph representation of A(b!1).B(a!1,c~0!2).B(b!2)
  release_assert(bng_data.get_seed_species().size() == 1);
  const Cplx& cplx = bng_data.get_seed_species()[0].cplx;

  const Graph& g = cplx.get_graph();

  dump_graph(g);

  // get count of vertices by iterating over them
  uint count = 0;

  typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
  pair<vertex_iter, vertex_iter> it;
  for (it = boost::vertices(g); it.first != it.second; ++it.first) {
    count++;
  }

  release_assert(count == 7);
}
