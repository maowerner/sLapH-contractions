#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Operator.h"

namespace LapH {

void check_random_combinations(std::string const &diagram,
                               std::vector<size_t> const &lookup,
                               std::vector<RandomIndexCombinationsQ2> const &ricQ2_lookup,
                               std::vector<VdaggerVRandomLookup> const &rvdaggervr_lookup,
                               std::vector<QuarklineQ2Indices> const &Q2V_lookup){
   const auto &ric0 =
       ricQ2_lookup[Q2V_lookup[lookup[0]].id_ric_lookup]
           .rnd_vec_ids;
   const auto &ric1 =
      ricQ2_lookup[rvdaggervr_lookup[lookup[1]].id_ricQ_lookup].rnd_vec_ids;
   const auto &ric2 =
       ricQ2_lookup[Q2V_lookup[lookup[2]].id_ric_lookup].rnd_vec_ids;
   const auto &ric3 =
       ricQ2_lookup[rvdaggervr_lookup[lookup[3]].id_ricQ_lookup].rnd_vec_ids;

   if (ric0.size() != ric1.size() || ric0.size() != ric2.size() ||
       ric0.size() != ric3.size()) {
     std::string error_message = 
       std::string("rnd combinations are not the same in ") + diagram;
     throw std::length_error(error_message);
   }
}

/*! 
 *  Multiply Operator with 2 Quarks and Operator with zero quarks
 *
 *  Dependency inversion principle: This interface will be a policy that must be 
 *  fullfilled by any linear algebra library to be used. 
 *
 *  @todo Layer of abstraction for Eigen in call parameter result
 */
template <>
void Q2xrVdaggerVr<QuarkLineType::Q2V>(std::vector<Eigen::MatrixXcd> &result, 
                    QuarkLineBlock<QuarkLineType::Q2V> const &quarklines,
                    OperatorsForMesons const &meson_operator,
                    int const b2,
                    int const t1,
                    int const t2,
                    std::array<size_t, 3> const look,
                    std::vector<RandomIndexCombinationsQ2> const &ricQ2_lookup,
                    std::vector<VdaggerVRandomLookup> const &rvdaggervr_lookup,
                    std::vector<QuarklineQ2Indices> const &Q2V_lookup,
                    size_t const dilE,
                    size_t const dilD){

  /*! Assume full dilution in Dirac space in the Loop over d */
  assert(dilD = 4);

  const auto &ric0 =
      ricQ2_lookup[Q2V_lookup[look[1]].id_ric_lookup].rnd_vec_ids;
  const auto &ric1 =
      ricQ2_lookup[rvdaggervr_lookup[look[2]].id_ricQ_lookup].rnd_vec_ids;

  size_t result_rnd_counter = 0;
  for (const auto &rnd0 : ric0) {
    for (const auto &rnd1 : ric1) {
      if (rnd0.first != rnd1.first && rnd0.second == rnd1.second) {

        /*! @Note Allocation should be refactored */
        result.emplace_back(Eigen::MatrixXcd::Zero(dilE * dilD, dilE * dilD));

        const size_t idr0 = &rnd0 - &ric0[0];
        const size_t idr1 = &rnd1 - &ric1[0];
        for (size_t d = 0; d < 4; d++) {
          // TODO: gamma hardcoded
          const cmplx value = quarklines.return_gamma_val(5, d);  
          const size_t gamma_index = quarklines.return_gamma_row(5, d);  
          //          const cmplx value =
          //          quarklines.return_gamma_val(c_look.gamma[0], d);
          //          const size_t gamma_index = quarklines.return_gamma_row(
          //                                                          c_look.gamma[0],
          //                                                          d);
          result[result_rnd_counter].block(0, d * dilE, dilD * dilE, dilE) =
              value *
              quarklines(
                  t1, b2, look[1], idr0)
                  .block(0, gamma_index * dilE, dilD * dilE, dilE) *
              meson_operator.return_rvdaggervr(look[2], t2, idr1)
                  .block(gamma_index * dilE, d * dilE, dilE, dilE);
        }
        result_rnd_counter++;
      }
    }
  }
}

void M1xM2(Eigen::MatrixXcd &result, 
           Eigen::MatrixXcd const &M1, 
           std::vector<Eigen::MatrixXcd> const &M2, 
           std::vector<size_t> const &lookup,
           std::vector<RandomIndexCombinationsQ2> const &ricQ2_lookup,
           std::vector<VdaggerVRandomLookup> const &rvdaggervr_lookup,
           std::vector<QuarklineQ2Indices> const &Q2V_lookup,
           std::pair<size_t, size_t> const & rnd0,
           std::pair<size_t, size_t> const & rnd1,
           size_t const dilE,
           size_t const dilD){

   const auto &ric2 = ricQ2_lookup[Q2V_lookup[lookup[2]].id_ric_lookup].rnd_vec_ids;
   const auto &ric3 = ricQ2_lookup[rvdaggervr_lookup[lookup[3]].id_ricQ_lookup].rnd_vec_ids;

   size_t M2_rnd_counter = 0;
   for (const auto &rnd2 : ric2) {
     for (const auto &rnd3 : ric3) {
       if (rnd2.first != rnd3.first && rnd2.second == rnd3.second) {

         // Check that no random vector is used in M1 and M2 at the same 
         // time
         if (rnd1.first == rnd2.first && rnd0.first == rnd3.first &&
             rnd0.second != rnd3.second) {
           result += M2[M2_rnd_counter];
         }
         M2_rnd_counter++;
       }
     }
   }

   result = (M1 * result);
}

cmplx trace(std::vector<Eigen::MatrixXcd> const &M1, 
           std::vector<Eigen::MatrixXcd> const &M2, 
           std::vector<size_t> const &lookup,
           std::vector<RandomIndexCombinationsQ2> const &ricQ2_lookup,
           std::vector<VdaggerVRandomLookup> const &rvdaggervr_lookup,
           std::vector<QuarklineQ2Indices> const &Q2V_lookup,
           size_t const dilE,
           size_t const dilD){

  /*! @todo unnessary allocation */
   Eigen::MatrixXcd M3 = Eigen::MatrixXcd::Zero(dilE * dilD, dilE * dilD);
   cmplx result = cmplx(.0,.0);

   const auto &ric0 = ricQ2_lookup[Q2V_lookup[lookup[0]].id_ric_lookup].rnd_vec_ids;
   const auto &ric1 = ricQ2_lookup[rvdaggervr_lookup[lookup[1]].id_ricQ_lookup].rnd_vec_ids;
   size_t M1_rnd_counter = 0;
   for (const auto &rnd0 : ric0) {
     for (const auto &rnd1 : ric1) {
       if (rnd0.first != rnd1.first && rnd0.second == rnd1.second) {
         // setting matrix values to zero
          M3.setZero(dilE * 4, dilE * 4);

          M1xM2(M3, M1[M1_rnd_counter], M2, lookup, ricQ2_lookup, rvdaggervr_lookup, 
                Q2V_lookup, rnd0, rnd1, dilE, 4);

          result += M3.trace();
          ++M1_rnd_counter;
       }
     }
   }

  return result;
}

}  // end of LapH namespace
