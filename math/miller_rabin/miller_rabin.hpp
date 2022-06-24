#pragma once

#include "../../utils/head_template.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>

struct MillerRabin {
public:
  void FindFacs(int64_t n) {
    this->fnum_ = 0;
    FindFacsImpl(n);
  }
  MillerRabin(int check_times = 20) : check_times_(check_times) {}

  // miller_rabin, return `true` if prime
  // max wrong rate: 1 / (4 ^ check_times_)
  bool IsPrime(int64_t n) const {
    if (n == 2)
      return 1;
    if (n < 2 || !(n & 1))
      return 0;
    int64_t x = n - 1;
    int64_t t = 0;
    while ((x & 1) == 0) {
      x >>= 1;
      t++;
    }
    for (int i = 0; i < check_times_; i++) {
      int64_t a = random() % (n - 1) + 1;
      if (IsComposite(a, n, x, t))
        return 0;
    }
    return 1;
  }

  size_t fnum() const { return fnum_; }
  const int64_t *factors() const { return factors_; }

private:
  // miller_rabin, return `true` if composite
  bool IsComposite(int64_t a, int64_t n, int64_t x, int64_t t) const {
    int64_t ret = OperatorWithModulo<int64_t, INT128>::pow_mod(a, x, n);
    int64_t last = ret;
    for (int i = 1; i <= t; i++) {
      ret = OperatorWithModulo<int64_t, INT128>::mul_mod(ret, ret, n);
      if (ret == 1 && last != 1 && last != n - 1)
        return 1;
      last = ret;
    }
    if (ret != 1)
      return 1;
    return 0;
  }

  int64_t gcd(int64_t a, int64_t b) {
    if (a == 0)
      return 1;
    if (a < 0)
      return gcd(-a, b);
    while (b) {
      int64_t t = a % b;
      a = b;
      b = t;
    }
    return a;
  }

  int64_t PollardsRho(int64_t x, int64_t c) {
    int64_t i = 1, k = 2;
    int64_t x0 = random() % x;
    int64_t y = x0;
    while (1) {
      i++;
      x0 = (OperatorWithModulo<int64_t, INT128>::mul_mod(x0, x0, x) + c) % x;
      int64_t d = gcd(y - x0, x);
      if (d != 1 && d != x)
        return d;
      if (y == x0)
        return x;
      if (i == k) {
        y = x0;
        k += k;
      }
    }
  }

  void FindFacsImpl(int64_t n) {
    if (IsPrime(n)) {
      factors_[fnum_++] = n;
      return;
    }
    int64_t p = n;
    while (p >= n)
      p = PollardsRho(p, random() % (n - 1) + 1);
    FindFacsImpl(p);
    FindFacsImpl(n / p);
  }

  const int check_times_;
  // pollard_rho
  int64_t factors_[100]; // factorization no order
  int fnum_;             // num of prime factor
};

#ifndef NDEBUG

static void _test_miller_rabin() {
  MillerRabin m;

  const auto is_prime = [](int64_t num) {
    int64_t mid = std::sqrt(num);
    for (int64_t x = 2; x <= mid; ++x) {
      if (num % x == 0)
        return 0;
    }
    return 1;
  };

  int t = 50;
  while (t--) {
    auto num = random() % int64_t(1e9) + 1;
    if (m.IsPrime(num)) {
      assert(is_prime(num));
    } else {
      assert(!is_prime(num));
    }
  }
}

#endif