/******************************************************************************
 * Copyright (C) 2020-2021 by
 * The Salk Institute for Biological Studies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
******************************************************************************/

/**
 * Definitions shared among BNG and other sources that might use it.
 *
 * No namespace is used here (except for float_t that in enclosed in namespace Common).
 */

#ifndef __BNG_DEFINES_SHARED_H__
#define __BNG_DEFINES_SHARED_H__

#include <stdint.h>
#include <climits>
#include <cassert>
#include <cfloat>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <cmath>
#include <chrono>
#include <ctime>

#include <boost/container/small_vector.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>

#include <google/dense_hash_map>
#include <google/dense_hash_set>

#ifdef _WIN64
typedef unsigned int uint;
#endif

// ---------------------------------- debug ----------------------------------------

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCTION__

// pybind11\detail/common.h(117,1):
// fatal error C1189: #error:  Macro clash with min and max -- define NOMINMAX when compiling your program on Windows
#define NOMINMAX

// casts
#pragma warning (disable : 4244)
#pragma warning (disable : 4267)
// unsafe functions
#pragma warning (disable : 4996)

#endif

// assert not conditioned by NDEBUG
#define release_assert(expr) \
  (static_cast <bool> (expr)                                            \
      ? void (0)                                                        \
      : __assert_fail_bng(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__))

static inline void
__assert_fail_bng (const char *assertion, const char *file, unsigned int line,
         const char *function)
{
  // assuming that the assertion is internal and malloc and other parts still work
  std::cerr << file << ":" << line << ": " << function << ": Assertion `" << assertion << "' failed.\n";
  std::flush(std::cout);
  abort();
}

//#define ENABLE_PERF_OUTPUT

// ---------------------------------- float types ----------------------------------

#define POS_T_BYTES 8

// float_t is also defined in mathdef.h, we need to enclose it into
// a namespace and then copy the definition in all define headers
namespace BNGCommon {

typedef double float_t;

const float_t EPS = 1e-12; // same as EPS_C
const float_t SQRT_EPS = 1e-6;
const float_t FLT_GIGANTIC = 1e140;

const float_t BNG_PI = 3.14159265358979323846;
const float_t BNG_N_AV = 6.0221417930e23;

#if POS_T_BYTES == 8

typedef double pos_t;
typedef double stime_t; // short time

const pos_t POS_EPS = EPS;
const pos_t POS_SQRT_EPS = SQRT_EPS;
const pos_t POS_FLT_GIGANTIC = FLT_GIGANTIC;

const stime_t STIME_EPS = EPS;
const pos_t STIME_SQRT_EPS = SQRT_EPS;
const stime_t STIME_FLT_GIGANTIC = FLT_GIGANTIC;
#define CHECK_STIME_MAX(v) do { } while (0)

#else
typedef float pos_t;
typedef float stime_t;

const pos_t POS_EPS = 1e-6f;
const pos_t POS_SQRT_EPS = 1e-4f;
const pos_t POS_FLT_GIGANTIC = 1e38f;

const stime_t STIME_EPS = 1e-6f;
const stime_t STIME_FLT_GIGANTIC = 1e-6f;
const stime_t STIME_MAX = 100.0f;
#define CHECK_STIME_MAX(v) do { assert((v) < BNGCommon::STIME_MAX); } while (0)
#endif

}


#include "bng/bng_config.h"

namespace BNGCommon {

// NOTE: we must be sure to use our BNGCommon::float_t here otherwise
// there will be conversion imprecisions if float_t from 'math.h' is used
std::string f_to_str(const float_t val, const int n = 17);

static inline float_t fabs_f(const float_t x) {
  return fabs(x);
}

static inline pos_t fabs_p(const pos_t x) {
#if POS_T_BYTES == 4
  return fabsf(x);
#else
  return fabs(x);
#endif
}

// TODO32: go through all usages and make sure that we are using the right type
template<typename T>
inline bool cmp_eq(const T& a, const T& b, const T eps) {
  return fabs_f(a - b) < eps;
}

template<typename T>
inline bool cmp_eq(const T& a, const T& b) {
  return cmp_eq(a, b, EPS);
}

template<>
inline bool cmp_eq(const pos_t& a, const pos_t& b) {
  return cmp_eq(a, b, POS_EPS);
}

// TODO32: can we use templates instead
// returns true when whether two values are measurably different
// TODO32: check all usages
// TODO: what about a simple comparison with eps?
static inline bool distinguishable_f(float_t a, float_t b, float_t eps) {
  float_t c = fabs_f(a - b);
  a = fabs_f(a);
  if (a < 1) {
    a = 1;
  }
  b = fabs_f(b);

  if (b < a) {
    eps *= a;
  } else {
    eps *= b;
  }
  return (c > eps);
}

// TODO: what about a simple comparison with eps?
static inline bool distinguishable_p(pos_t a, pos_t b, pos_t eps) {
  float_t c = fabs_p(a - b);
  a = fabs_p(a);
  if (a < 1) {
    a = 1;
  }
  b = fabs_p(b);

  if (b < a) {
    eps *= a;
  } else {
    eps *= b;
  }
  return (c > eps);
}

// TODO32: remove these after changing float_t to double
static inline float_t sqrt_f(const float_t x) {
  return sqrt(x);
}


static inline float_t pow_f(const float_t a, const float_t n) {
  return pow(a, n);
}


static inline float_t floor_f(const float_t a) {
  return floor(a);
}


static inline float_t round_f(const float_t a) {
  return round(a);
}

} // namespace BNGCommon


// ---------------------------------- fixed constants and specific typedefs -------------------
const float_t POS_INVALID = FLT_MAX; // cannot be NAN because we cannot do any comparison with NANs

const float_t FLT_INVALID = FLT_MAX;

const float_t TIME_INVALID = -1;
const float_t TIME_FOREVER = FLT_MAX; // this max is sufficient for both float and double
const float_t TIME_SIMULATION_START = 0;

const float_t DEFAULT_TIME_STEP = 1.0;

const uint UINT_INVALID = UINT32_MAX; // invalid value to be used for any invalid unsigned integer values
const uint UINT_INVALID2 = UINT32_MAX - 1; // second invalid value not to be used, in this case for any purpose
const int INT_INVALID = INT32_MAX;

const uint ID_INVALID = UINT_INVALID; // general invalid index, should not be used when a definition for a specific type is available
const uint ID_INVALID2 = UINT32_MAX - 1; // second general invalid id, might be used for special values
const uint INDEX_INVALID = UINT_INVALID; // general invalid index, should not be used when a definition for a specific type is available
const uint INDEX_INVALID2 = UINT32_MAX - 2; // second general invalid index, might be used for special values

// molecule id is a unique identifier of a molecule,
// no 2 molecules may have the same ID in the course of a simulation
// not used from the BNG lib yet
typedef uint molecule_id_t;
const molecule_id_t MOLECULE_ID_INVALID = ID_INVALID;

// unique species id,
// every distinct species that exists
// TODO: move to bng?
typedef uint species_id_t;
const species_id_t SPECIES_ID_INVALID = ID_INVALID;

typedef uint species_index_t;
const species_index_t SPECIES_INDEX_INVALID = ID_INVALID;

// unique pattern id
typedef uint pattern_id_t;
const pattern_id_t PATTERN_ID_INVALID = ID_INVALID;

typedef int32_t orientation_t;
const orientation_t ORIENTATION_DOWN = -1;
const orientation_t ORIENTATION_NONE = 0;
const orientation_t ORIENTATION_UP = 1;
const orientation_t ORIENTATION_NOT_SET = 2;
// needed for handling of reaction in the form S(s!1).V(v!1) -> V(s) + S(s),
// used only for volume products of a reaction
const orientation_t ORIENTATION_DEPENDS_ON_SURF_COMP = 3;

char orientation_to_char(const orientation_t o);

// index of reaction with products in a reaction class
typedef int rxn_class_pathway_index_t;
// TODO: originally RX_NO_RX, fix all cases in MCell
const rxn_class_pathway_index_t PATHWAY_INDEX_LEAST_VALID = 0;
const rxn_class_pathway_index_t PATHWAY_INDEX_NO_RXN = -2;
const rxn_class_pathway_index_t PATHWAY_INDEX_INVALID = -11; // just some ad-hoc value, MCell3 might be using -1

// index of a reaction classes in an array of reaction classes
typedef int rxn_class_index_t;
const rxn_class_index_t RNX_CLASS_INDEX_INVALID = INT_INVALID;

#ifndef NDEBUG
// FIXME: disable this even for debug build
#define INDEXER_WA // Don't know yet how to convince Eclipse to correctly index boost containers
#endif

#if defined(NDEBUG) && defined(INDEXER_WA)
#warning "INDEXER_WA is enabled and this will lead to lower performance"
#endif


#ifndef INDEXER_WA
template<class T, class Allocator=boost::container::new_allocator<T>>
  using small_vector = boost::container::small_vector<T, 8, Allocator>;

template<class T, typename Compare = std::less<T>, class Allocator=boost::container::new_allocator<T>>
  using base_flat_set = boost::container::flat_set<T, Compare, Allocator>;
#else
template<typename T, typename _Alloc = std::allocator<T>  >
  using small_vector = std::vector<T, _Alloc>;

template<typename T, typename _Compare = std::less<T>, typename _Alloc = std::allocator<T>  >
  using base_flat_set = std::set<T, _Compare, _Alloc>;
#endif


static inline std::ostream& errs() {
  std::cerr << "error: ";
  return std::cerr;
}

static inline std::ostream& warns() {
  std::cout << "warning: ";
  return std::cout;
}

static inline std::ostream& notifys() {
  std::cout << "notification: ";
  return std::cout;
}

class NullBuffer: public std::streambuf
{
public:
  int overflow(int c) { return c; }
};

static inline std::ostream& perf() {
#ifdef ENABLE_PERF_OUTPUT
  std::time_t now = std::time(nullptr);
  char time_str[64];
  std::strftime(time_str, sizeof(time_str), "%M:%S", std::localtime(&now));
  std::cout << "perf: (" << time_str << ") ";
  return std::cout;
#else
  static NullBuffer null_buffer;
  static std::ostream null_stream(&null_buffer);
  return null_stream;
#endif
}

/**
 * Set based on boost's flat_set. 
 * Used to hold sets of ids or indices of molecules or other items,
 * extended to check for unique insertions and checked removals.
 *
 * T is really not needed since it is an uint, but the declaration then
 * shows what type of values are expected.
 */
template<typename Key>
class uint_set: public base_flat_set<Key> {
public:
  // insert with check that the item is not there yet
  // for insertions without this check use 'insert'
  void insert_unique(const Key id_or_index) {
    assert(this->count(id_or_index) == 0);
    //release_assert(this->count(id_or_index) == 0);
    this->insert(id_or_index);
  }

  // erase with check that the item is present
  // for insertions without this check use 'erase'
  void erase_existing(const Key id_or_index) {
    assert(this->count(id_or_index) == 1);
    //release_assert(this->count(id_or_index) == 1);
    this->erase(id_or_index);
  }

  void shrink() {
#ifdef NDEBUG
      // INDEXER_WA must be disabled for this to compile
      this->shrink_to_fit();
#else
      base_flat_set<molecule_id_t> tmp = *this;
      this->swap(tmp);
#endif
  }

  void dump(const std::string comment = "") const;
};


/**
 * Set based on googles's dense_hash_set.
 */
template<typename Key>
class uint_dense_hash_set: public google::dense_hash_set<Key> {
public:
  uint_dense_hash_set() {
    // dense_hash_set requires this to be called
    this->set_empty_key(UINT_INVALID);
    this->set_deleted_key(UINT_INVALID2);
  }

  // insert with check that the item is not there yet
  // for insertions without this check use 'insert'
  void insert_unique(const Key id_or_index) {
    assert(this->count(id_or_index) == 0);
    this->insert(id_or_index);
  }

  // erase with check that the item is present
  // for insertions without this check use 'erase'
  void erase_existing(const Key id_or_index) {
    assert(this->count(id_or_index) == 1);
    this->erase(id_or_index);
  }
};


/**
 * Map based on boost's flat_map.
 * 
 * Not used yet
 */ 
template<typename Key, typename Value>
class uint_flat_map: public boost::container::flat_map<Key, Value> {

};

/**
 * Map based on googles's dense_hash_map.
 * 
 *  Warning: value must be POD (plain old data), this is not checked during compilations.
 *
 *  Not used yet
 */
template<typename Key, typename Value>
class uint_dense_hash_map: public google::dense_hash_map<Key, Value> {
public:
  uint_dense_hash_map() {
    assert(sizeof(Value) <= 8);
    // dense_hash_map requires this to be called
    this->set_empty_key(UINT_INVALID);
    this->set_deleted_key(UINT_INVALID2);
  }
};


template<typename T>
void dump_container(const T& s, std::string comment = "") {
  std::cout << comment << ": ";
  int cnt = 0;
  for (auto value: s) {
    std::cout << value << ", ";

    if (cnt %20 == 0 && cnt != 0) {
      std::cout << "\n";
    }
    cnt++;
  }
  std::cout << "\n";
}


template<typename Key>
void uint_set<Key>::dump(const std::string comment) const {
  dump_container(*this, comment);
}


#endif // __BNG_DEFINES_SHARED_H__
