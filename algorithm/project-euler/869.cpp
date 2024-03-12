#include <algorithm/math/Template/prime_factors.hpp>

struct Tire {
  static constexpr size_t N = 2;

  void build(U64 n) {
    Tire *p = this;
    for (U64 x = n; x; x >>= 1) {
      Tire *u = p->next(x & 1);
      u->cnt_++;
      p = u;
    }
  }

  double search(U64 n) {
    Tire *p = this;
    double res = 0;
    for (U64 x = n; x; x >>= 1) {
      auto c1 = p->next_cnt(1);
      auto c0 = p->next_cnt(0);
      if (c1 > c0) {
        if (x & 1)
          res += 1;
      } else if (c1 < c0) {
        if (!(x & 1))
          res += 1;
      } else {
        res += 0.5;
      }
      p = p->next(x & 1);
    }
    DEBUG_FMSGLN("{}: {}", n, res);
    return res;
  }

  Tire *next(int c) {
    if (!nxt_[c]) {
      nxt_[c] = new ((Tire *)fast_bin_.Alloc()) Tire{fast_bin_};
    }
    return nxt_[c];
  }

  U64 next_cnt(int c) {
    if (!nxt_[c]) {
      return 0;
    }
    return nxt_[c]->cnt_;
  }

  utils::FastBin<Tire> &fast_bin_;
  Tire *nxt_[2]{};
  U64 cnt_{};
};

int main() {
  SHOW_TIME_COST();

  utils::FastBin<Tire> fast_bin;
  Tire root{fast_bin};
  size_t n = 1e8;
  DEBUG_SCOPE({ n = 30; });
  PrimeHelper prime_helper = PrimeHelper::GenPrimeHelperWithMaxLen(n);
  DEBUG_FMSGLN("number of primes: {}", prime_helper.primes().size());
  {
    SHOW_TIME_COST("build prime tire");
    for (auto prime : prime_helper.primes()) {
      root.build(prime);
      DEBUG_FMSGLN("{:>5}:{:>15b}", prime, prime);
    }
  }
  double res = 0;
  {
    SHOW_TIME_COST("search prime tire");
    for (auto prime : prime_helper.primes()) {
      res += root.search(prime);
    }
  }
  FMSGLN("{:.8f}", res / prime_helper.primes().size());
  return 0;
}