/******************************************************************************
 * Copyright (C) 2020-2021 by
 * The Salk Institute for Biological Studies
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
******************************************************************************/

#ifndef LIBS_BNG_BNG_CONFIG_H_
#define LIBS_BNG_BNG_CONFIG_H_

// ---------------------------------- configurability ------------------------------
/**
 * The goal of this library of for it to be general enough so that different
 * simulation engines can use it.
 *
 * One such example is what information does the specific engine needs about
 * species. E.g. MCell species uses variables space_step and time_step.
 * These are most probably of little use.
 *
 * One approach to configurability are templates. However the issue with templates
 * is that they make the code hard to read and practically all code must be in
 * headers which slows down compilation time.
 *
 * Another approach are virtual methods. Here, one would need to provide functions
 * to create instances of e.g. the Species object. Then, every time the object is
 * retrieved from the library, it needs to be cast to the right class.
 * Using vertial classes means that we must allocate each class and use pointers,
 * this might lead to "scattered?" memory usage compared to vectors.
 *
 * One more approach is to use preprocessor. This brings disadvantages that a big
 * part of code might not be compilable and different implementations can diverge.
 * For example one could use preprocessor conditioning must be used only in necessary
 * cases such as when defining attributes and methods for Species or Reactions,
 * however,
 *
 * Therefore, all the variables needed for species and reactions along with their
 * code for every simulator that uses this library is present here and shared.
 * We might rethink this later.
 *
 */
#define BNG_MCELL 1

// included only in defines_shared.h

namespace BNG {

// separate class because the number of notifications may grow in the future
class BNGNotifications {
public:
  BNGNotifications() :
    bng_verbosity_level(0),
    rxn_probability_changed(true)
    {
  }

  // print additional information during simulation
  int bng_verbosity_level;

  // same as MCell3's varying_probability_report,
  bool rxn_probability_changed;

  void dump() const;
};


class BNGWarnings {
public:
  BNGWarnings() :
    warn_on_bimol_rxn_probability_over_05_less_1(true),
    bimol_rxn_probability_over_05_less_1(false),
    bimol_rxn_probability_over_1(false)
    {
  }

  // warning settings
  bool warn_on_bimol_rxn_probability_over_05_less_1;

  // status
  mutable bool bimol_rxn_probability_over_05_less_1; // set to true if probability of a bimol rxn was > 0.5 but < 1
  mutable bool bimol_rxn_probability_over_1; // set to true if probability of a bimol rxn was > 1

};


class BNGConfig {
public:
  BNGConfig() :
    mcell(BNG_MCELL),
    initial_seed(1),
    use_bng_units(false),
    time_unit(0),
    length_unit(0),
    rcp_length_unit(0),
    grid_density(0),
    rxn_radius_3d(0),
    intermembrane_rxn_radius_3d(0),
    rxn_and_species_report(true),
	  debug_requires_diffusion_constants(false)
    {
  }

  // configuration
  bool mcell;

  uint initial_seed;

  /*
  When use_bng_units is False (default), traditional MCell units are used:
    * [M^-1*s^-1] for bimolecular reactions between either two volume molecules, a volume molecule
                  and a surface (molecule),
    * [um^2*N^-1*s^-1] bimolecular reactions between two surface molecules on the same surface, and
  When use_bng_units is True, units compatible with BioNetGen's ODE, SSA, and PLA solvers are used:
    * [um^3*N^-1*s^-1] for any bimolecular reactions, surface-surface reaction rate conversion assumes 10nm membrane thickness.

  This setting only impacts the computation of reaction probability where, if use_bng_units is true,
  BNG units are first converted to MCell units and then the probability factor is computed.
  */
  bool use_bng_units;

  // MCell
  double time_unit;
  BNGCommon::pos_t length_unit;
  BNGCommon::pos_t rcp_length_unit;
  BNGCommon::pos_t grid_density;
  BNGCommon::pos_t rxn_radius_3d;
  BNGCommon::pos_t intermembrane_rxn_radius_3d;

  // generate report files during simulation
  bool rxn_and_species_report;

  // flag to enable debug assertions that check that diffusion constants are set
  // default is false
  bool debug_requires_diffusion_constants;

  BNGWarnings warnings;
  BNGNotifications notifications;


  void init() {
    rcp_length_unit = 1/length_unit;
    initialize_bng_report_files();
  }

  std::string get_rxn_report_file_name() const;
  std::string get_species_report_file_name() const;
  std::string get_warnings_report_file_name() const;

  // prints warnings at the end of simulation if needed (to the standard error output)
  void print_final_warnings() const;

  void dump() const;

  BNGCommon::pos_t get_default_rxn_radius_3d() const;

protected:
  std::string seed_as_str() const;
  void initialize_bng_report_files();
};


// report utilities
std::string get_current_date_time();
void append_to_report(const std::string& report_fname, const std::string& msg);
void initialize_report_file(const std::string& fname, const char* report_name);
void remove_report_file(const std::string& fname, const char* report_name);

} // namespace BNG


#endif /* LIBS_BNG_BNG_CONFIG_H_ */
