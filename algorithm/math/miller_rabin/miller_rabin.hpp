#pragma once

#include <utils/utils.h>

struct MillerRabin {
  using Modulo = utils::OperatorWithModulo<int64_t, I128>;

public:
  std::vector<int64_t> FindFacs(int64_t n) {
    std::vector<int64_t> factors; // factorization no order
    FindFacsImpl(n, [&](int64_t n) { factors.emplace_back(n); });
    return factors;
  }

  template <typename F> void FindFacs(int64_t n, F &&handle_fac) {
    FindFacsImpl(n, std::forward<F>(handle_fac));
  }

  MillerRabin(int check_times = 20) : check_times_(check_times) {}

  // miller_rabin, return `true` if prime
  // max wrong rate: 1 / (4 ^ check_times_)
  bool IsPrime(int64_t n) const {
    if (n == 2)
      return 1;
    if (n < 2 || !(n & 1))
      return 0;
    int64_t d = n - 1;
    int t = 0;
    while ((d & 1) == 0) {
      d >>= 1;
      t++;
    }
    for (int i = 0; i < check_times_; i++) {
      int64_t a = random() % (n - 1) + 1;
      if (!MillerRabinTest(a, n, d, t))
        return false;
    }
    return true;
  }

private:
  // miller_rabin, return `false` if composite
  bool MillerRabinTest(const int64_t a, const int64_t n, const int64_t d,
                       const int t) const {
    int64_t x = Modulo::pow_mod(a, d, n);
    if (x == 1 || x == n - 1)
      return true;
    rp(_, t) {
      x = Modulo::mul_mod(x, x, n);
      if (x == 1)
        return false;
      if (x == n - 1)
        return true;
    }
    return false;
  }

  // pollard_rho algorithm
  int64_t PollardsRho(int64_t x, int64_t c) {
    int64_t i = 1, k = 2;
    int64_t x0 = random() % x;
    int64_t y = x0;
    while (1) {
      i++;
      x0 = (Modulo::mul_mod(x0, x0, x) + c) % x;
      int64_t d = std::gcd(y - x0, x);
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

  template <typename F> void FindFacsImpl(int64_t n, F &&handle_fac) {
    if (IsPrime(n)) {
      handle_fac(n);
      return;
    }
    int64_t p = n;
    while (p >= n)
      p = PollardsRho(p, random() % (n - 1) + 1);
    FindFacsImpl(p, std::forward<F>(handle_fac));
    FindFacsImpl(n / p, std::forward<F>(handle_fac));
  }

private:
  const int check_times_;
};
