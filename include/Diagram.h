#pragma once

#include "QuarkLineBlock2.h"
#include "typedefs.h"
#include "Correlators.h"

struct QuarkLineBlockCollection {
  QuarkLineBlockCollection(RandomVector const &random_vector,
                           Perambulator const &perambulator,
                           OperatorsForMesons const &meson_operator,
                           size_t const dilT,
                           size_t const dilE,
                           size_t const nev,
                           DilutedFactorLookup const &dil_fac_lookup,
                           DilutedTraceCollection<2> &corr0,
                           DilutedTraceCollection<2> &corrC,
                           DilutedTraceCollection2<1> &corr_part_trQ1)
      : q0(random_vector,
           perambulator,
           meson_operator,
           dilT,
           dilE,
           nev,
           dil_fac_lookup.Q0),
        q1(random_vector,
           perambulator,
           meson_operator,
           dilT,
           dilE,
           nev,
           dil_fac_lookup.Q1),
        q2l(random_vector,
            perambulator,
            meson_operator,
            dilT,
            dilE,
            nev,
            dil_fac_lookup.Q2L),
        q2v(random_vector,
            perambulator,
            meson_operator,
            dilT,
            dilE,
            nev,
            dil_fac_lookup.Q2V),
        corr0(corr0),
        corrC(corrC),
        corr_part_trQ1(corr_part_trQ1) {}

  void clear() {
    q0.clear();
    q1.clear();
    q2l.clear();
    q2v.clear();
  }

  QuarkLineBlock2<QuarkLineType::Q0> q0;
  QuarkLineBlock2<QuarkLineType::Q1> q1;
  QuarkLineBlock2<QuarkLineType::Q2> q2l;
  QuarkLineBlock2<QuarkLineType::Q2> q2v;

  DilutedTraceCollection<2> &corr0;
  DilutedTraceCollection<2> &corrC;
  DilutedTraceCollection2<1> &corr_part_trQ1;
};

class Diagram {
 public:
  Diagram(std::vector<CorrInfo> const &corr_lookup) : corr_lookup_(corr_lookup) {}

  virtual ~Diagram() {}

  virtual char const *name() const = 0;

  std::vector<CorrInfo> const &corr_lookup() const { return corr_lookup_; }

  void build(std::string const output_path,
             std::string const output_filename,
             const size_t Lt,
             const size_t dilT, QuarkLineBlockCollection &q) {
    build_impl(output_path, output_filename, Lt, dilT, q);
  }

 private:
  std::vector<CorrInfo> const &corr_lookup_;

  virtual void build_impl(std::string const output_path,
                          std::string const output_filename,
                          const size_t Lt,
                          const size_t dilT, QuarkLineBlockCollection &q) = 0;
};

class DiagramComp : public Diagram {
 public:
  DiagramComp(std::vector<CorrInfo> const &corr_lookup) : Diagram(corr_lookup) {}

  void contract(std::vector<cmplx> &c,
                BlockIterator const &slice_pair,
                QuarkLineBlockCollection &q) {
    contract_impl(c, slice_pair, q);
  }

 private:
  virtual void contract_impl(std::vector<cmplx> &c,
                             BlockIterator const &slice_pair,
                             QuarkLineBlockCollection &q) = 0;

  void build_impl(std::string const output_path,
                  std::string const output_filename,
                  const size_t Lt,
                  const size_t dilT, QuarkLineBlockCollection &q) override {
    build_diagram<cmplx>(*this, output_path, output_filename, Lt, dilT, q);
  }
};

class DiagramCompComp : public Diagram {
 public:
  DiagramCompComp(std::vector<CorrInfo> const &corr_lookup) : Diagram(corr_lookup) {}

  void contract(std::vector<compcomp_t> &c,
                BlockIterator const &slice_pair,
                QuarkLineBlockCollection &q) {
    contract_impl(c, slice_pair, q);
  }

 private:
  virtual void contract_impl(std::vector<compcomp_t> &c,
                             BlockIterator const &slice_pair,
                             QuarkLineBlockCollection &q) = 0;

  void build_impl(std::string const output_path,
                  std::string const output_filename,
                  const size_t Lt,
                  const size_t dilT, QuarkLineBlockCollection &q) override {
    build_diagram<compcomp_t>(*this, output_path, output_filename, Lt, dilT, q);
  }
};

/*****************************************************************************/
/*                                    C2                                     */
/*****************************************************************************/

class C2c : public DiagramComp {
 public:
  C2c(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C2+"; }

 private:
  void contract_impl(std::vector<cmplx> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;
};

class C20 : public DiagramComp {
 public:
  C20(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C20"; }

 private:
  void contract_impl(std::vector<cmplx> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;
};


class C20V : public DiagramCompComp {
 public:
  C20V(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C20V"; }

 private:
  void contract_impl(std::vector<compcomp_t> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;
};

/*****************************************************************************/
/*                                    C3                                     */
/*****************************************************************************/

/*! Build neutral 3pt correlation function
 *  @f{align}{
 *    C = \langle \gamma_5 D_\mathtt{Q0}^{-1}(t|t)^\dagger \gamma_5 \Gamma_\mathtt{Op0}
 *                D_\mathtt{Q1}^{-1}(t|t') \Gamma_\mathtt{Op1}
 *                D_\mathtt{Q2}^{-1}(t'|t) \Gamma_\mathtt{Op2} \rangle
 *  @f}
 */
class C3c : public DiagramComp {
 public:
  C3c(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C3+"; }

 private:
  void contract_impl(std::vector<cmplx> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;

  std::vector<std::tuple<std::array<size_t, 2>, std::array<size_t, 1>>> quantum_num_ids_;
};

/*! Build neutral 3pt correlation function
 *  @f{align}{
 *    C = \langle D_\mathtt{Q0}^{-1}(t|t) \Gamma_\mathtt{Op0}
 *                D_\mathtt{Q1}^{-1}(t|t') \Gamma_\mathtt{Op1}
 *                D_\mathtt{Q2}^{-1}(t'|t) \Gamma_\mathtt{Op2} \rangle
 *  @f}
 */
class C30 : public DiagramComp {
 public:
  C30(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C30"; }

 private:
  void contract_impl(std::vector<cmplx> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;

  std::vector<std::tuple<std::array<size_t, 2>, std::array<size_t, 1>>> quantum_num_ids_;
};

/*****************************************************************************/
/*                                   C4                                     */
/*****************************************************************************/

class C4cD : public DiagramCompComp {
 public:
  C4cD(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C4+D"; }

 private:
  void contract_impl(std::vector<compcomp_t> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;
};

class C40D : public DiagramCompComp {
 public:
  C40D(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C40D"; }

 private:
  void contract_impl(std::vector<compcomp_t> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;
};

class C4cV : public DiagramCompComp {
 public:
  C4cV(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C4+V"; }

 private:
  void contract_impl(std::vector<compcomp_t> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;
};

class C40V : public DiagramCompComp {
 public:
  C40V(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C40V"; }

 private:
  void contract_impl(std::vector<compcomp_t> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;
};

/*! Build charged 4pt correlation function: Box diagram
 *  @f{align}{
 *    C = \langle \gamma_5 D_\mathtt{Q0}^{-1}(t|t)^\dagger \gamma_5 \Gamma_\mathtt{Op0}
 *                D_\mathtt{Q1}^{-1}(t|t') \Gamma_\mathtt{Op1}
 *                \gamma_5 D_\mathtt{Q2}^{-1}(t'|t')^\dagger \gamma_5 \Gamma_\mathtt{Op2}
 *                D_\mathtt{Q3}^{-1}(t'|t) \Gamma_\mathtt{Op3} \rangle
 *  @f}
 */
class C4cB : public DiagramComp {
 public:
  C4cB(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C4+B"; }

 private:
  void contract_impl(std::vector<cmplx> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;

  std::vector<std::array<std::array<size_t, 2>, 2>> quantum_num_ids_;
};

/*! Build neutral 4pt correlation function: Box diagram
 *  @f{align}{
 *    C = \langle D_\mathtt{Q0}^{-1}(t|t) \Gamma_\mathtt{Op0}
 *                D_\mathtt{Q1}^{-1}(t|t') \Gamma_\mathtt{Op1}
 *                D_\mathtt{Q2}^{-1}(t'|t') \Gamma_\mathtt{Op2}
 *                D_\mathtt{Q3}^{-1}(t'|t) \Gamma_\mathtt{Op3} \rangle
 *  @f}
 */
class C40B : public DiagramComp {
 public:
  C40B(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C40B"; }

 private:
  void contract_impl(std::vector<cmplx> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;

  std::vector<std::array<std::array<size_t, 2>, 2>> quantum_num_ids_;
};

/*! Build charged 4pt correlation function: Cross diagram
 *  @f{align}{
 *    C = \langle \gamma_5 D_\mathtt{Q0}^{-1}(t|t')^\dagger \gamma_5 \Gamma_\mathtt{Op0}
 *                D_\mathtt{Q1}^{-1}(t|t') \Gamma_\mathtt{Op1}
 *                \gamma_5 D_\mathtt{Q2}^{-1}(t|t')^\dagger \gamma_5 \Gamma_\mathtt{Op2}
 *                D_\mathtt{Q3}^{-1}(t|t') \Gamma_\mathtt{Op3} \rangle
 *  @f}
 */
class C4cC : public DiagramComp {
 public:
  C4cC(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C4+C"; }

 private:
  void contract_impl(std::vector<cmplx> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;

  std::vector<std::array<std::array<size_t, 2>, 2>> quantum_num_ids_;
};

/*! Build neutral 4pt correlation function: Cross diagram
 *  @f{align}{
 *    C = \langle D_\mathtt{Q0}^{-1}(t'|t) \Gamma_\mathtt{Op0}
 *                D_\mathtt{Q1}^{-1}(t|t') \Gamma_\mathtt{Op1}
 *                D_\mathtt{Q2}^{-1}(t'|t) \Gamma_\mathtt{Op2}
 *                D_\mathtt{Q3}^{-1}(t|t') \Gamma_\mathtt{Op3} \rangle
 *  @f}
 */
class C40C : public DiagramComp {
 public:
  C40C(std::vector<CorrInfo> const &corr_lookup);

  char const *name() const override { return "C40C"; }

 private:
  void contract_impl(std::vector<cmplx> &c,
                     BlockIterator const &slice_pair,
                     QuarkLineBlockCollection &q) override;

  std::vector<std::array<std::array<size_t, 2>, 2>> quantum_num_ids_;
};
