#pragma once

#include "global_data_typedefs.hpp"
#include "typedefs.hpp"

#include <sys/stat.h>
#include <array>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

using DiagramIndicesCollection = std::map<std::string, std::vector<DiagramIndex>>;
using TraceIndicesCollection = std::map<std::string, std::vector<Indices>>;

enum class Location { source, sink };

struct TraceRequest {
  std::string tr_name;
  ssize_t tr_id;
  std::vector<Location> locations;

  bool operator==(TraceRequest const &other) const {
    return tr_name == other.tr_name && tr_id == other.tr_id &&
           locations == other.locations;
  }
};

struct CorrelatorRequest {
  std::vector<TraceRequest> trace_requests;
  std::string hdf5_dataset_name;

  bool operator==(CorrelatorRequest const &other) const {
    return trace_requests == other.trace_requests;
  }
};

using CorrelatorRequestsMap = std::map<std::string, std::vector<CorrelatorRequest>>;

/**
 * Class containing all metadata for contractions and functions to set them
 * from infile
 *
 *  Metadata roughly characterized by either
 *
 *  - physical parameters
 *  - flags
 *  - paths
 */
struct GlobalData {
  GlobalData() {
    momentum_cutoff[0] = 4;
    momentum_cutoff[1] = 5;
    momentum_cutoff[2] = 6;
    momentum_cutoff[3] = 7;
    momentum_cutoff[4] = 4;

    quarkline_lookuptable["Q0"];
    quarkline_lookuptable["Q1"];
    quarkline_lookuptable["Q2"];

    trace_indices_map["trQ0Q2"];
    trace_indices_map["trQ1"];
    trace_indices_map["trQ1Q1"];
    trace_indices_map["trQ1Q1Q1Q1"];
    trace_indices_map["trQ2Q0Q2Q0"];
    trace_indices_map["trQ2Q0Q2Q0Q2Q0"];
  }

  int Lx, Ly, Lz, Lt;
  int dim_row, V_TS, V_for_lime;
  int number_of_eigen_vec;
  int number_of_inversions;
  int start_config, end_config, delta_config;
  int verbose;
  ssize_t nb_eigen_threads;
  std::string path_eigenvectors;
  std::string name_eigenvectors;
  std::string filename_eigenvectors;
  std::string path_perambulators;
  std::string name_perambulators;
  std::string name_lattice;
  std::string filename_ending_correlators;
  std::string path_output;
  std::string path_config;
  std::string handling_vdaggerv;
  std::string path_vdaggerv;

  RandomVectorConstruction rnd_vec_construct;
  PerambulatorConstruction peram_construct;

  std::vector<quark> quarks;
  Operator_list operator_list;
  Correlator_list correlator_list;
  DilutedFactorIndicesCollection quarkline_lookuptable;
  OperatorLookup operator_lookuptable;
  TraceIndicesCollection trace_indices_map;
  CorrelatorRequestsMap correlator_requests_map;

  /**
   * Cutoff for the sum of individual momenta.
   *
   * The index is the total momentum squared, @f$ |P|^2 @f$, the value the sum
   * of the individual momenta squared, @f$ |p_1|^2 + |p_2|^2 @f$. The values
   * are chosen by hand from a feeling about the signal quality. When building
   * momentum combinations to compute, the condition @f$ |p_1|^2 + |p_2|^2 \le
   * c(|P|^2) @f$ will be enforced.
   *
   * Signal quality gets worse with large individual momenta, therefore it does
   * not make sense to include a very large @f$ p_1 @f$ in the @f$ |P|^2 = 0
   * @f$ case (with @f$ p_2 = - p_1 @f$). The current cutoff of 4 means that
   * only individual momenta up to @f$ (0, 0, 2) @f$ are computed.
   */
  std::map<int, int> momentum_cutoff;

  HypPars hyp_parameters;
};

/**
 * Reading the input parameters from the infile in the main routine and
 * initializing GlobalData.
 */
void read_parameters(GlobalData &gd, int ac, char *av[]);

std::ostream &operator<<(std::ostream &os, GlobalData const &gd);
