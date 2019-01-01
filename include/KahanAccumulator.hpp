#pragma once

/**
 * Kahan summation implementation.
 *
 * The algorithm is taken from
 * https://en.wikipedia.org/wiki/Kahan_summation_algorithm
 *
 * @tparam Numeric A numeric type which supports addition and subtraction. The
 * most sensible values are `double` and `std::complex<double>`.
 */
template <typename Numeric>
class KahanAccumulator {
 public:
  Numeric value() const { return sum_; }

  KahanAccumulator &operator+=(Numeric const right) {
    Numeric const y = right - c_;
    Numeric const t = sum_ + y;
    c_ = (t - sum_) - y;
    sum_ = t;
    return *this;
  }

  KahanAccumulator operator+(Numeric const right) const {
    KahanAccumulator left = *this;
    return left += right;
  }

  KahanAccumulator operator+=(KahanAccumulator const right) {
    sum_ += right.sum_;
    c_ += right.c_;
    return *this;
  }

 private:
  Numeric sum_ = 0.0;
  Numeric c_ = 0.0;
};

template <typename Numeric>
class NativeAccumulator {
 public:
  Numeric value() const { return sum_; }

  NativeAccumulator operator+=(Numeric const right) {
    sum_ += right;
    return *this;
  }

  NativeAccumulator operator+(Numeric const right) const {
    NativeAccumulator left = *this;
    return left += right;
  }

  NativeAccumulator operator+=(NativeAccumulator const right) {
    sum_ += right.sum_;
    return *this;
  }

 private:
  Numeric sum_ = 0.0;
};

template <typename Numeric>
#ifdef SLAPH_KAHAN
using Accumulator = KahanAccumulator<Numeric>;
#else
using Accumulator = NativeAccumulator<Numeric>;
#endif
