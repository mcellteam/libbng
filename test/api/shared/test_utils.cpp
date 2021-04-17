
#include "test_utils.h"
#include "bng/bng.h"

using namespace std;

std::string get_test_bngl_file_name(const char* source_file) {
	// source_file is a full path obtained with __FILE__
	std::string src(source_file);

	size_t pos = src.find_last_of("/\\");
	release_assert(pos != string::npos);

	return src.substr(0, pos+1) + "test.bngl";
}
