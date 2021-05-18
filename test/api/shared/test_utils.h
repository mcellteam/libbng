
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>
#include <set>

namespace BNG {
class BNGEngine;
class RxnClass;
}

std::string get_test_bngl_file_name(const char* source_file);

void generate_network(BNG::BNGEngine& bng_engine, std::set<BNG::RxnClass*>& all_rxn_classes);

void dump_network(const std::set<BNG::RxnClass*>& all_rxn_classes);

#endif // TEST_UTILS_H
