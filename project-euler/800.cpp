#include "math/Template/prime_factors.hpp"

static double foo(int64_t p, int64_t q) { return q * log2(p) + p * log2(q); }

int main() {
  auto f1 = 800800 * log2(800800);
  auto f2 = 800 * log2(800);
  auto f3 = log2(800);
  auto &&f = f1;
  auto _primes = Prime::GenPrimeWithMaxLen(f);
  auto &&primes = _primes.primes();
  int64_t n = 0;
  rp(start, primes.size() - 1) {
    auto p = primes[start];
    size_t bg = start + 1, ed = primes.size();
    while (bg != ed) {
      auto mid = (bg + ed) / 2;
      auto q = primes[mid];
      auto v = foo(p, q);
      if (v <= f) {
        bg = mid + 1;
      } else {
        ed = mid;
      }
    }
#ifndef NDEBUG
    if (ed > start + 1) {
      auto s = foo(p, primes[ed]);
      auto s2 = foo(p, primes[ed - 1]);
      assert(s > f);
      assert(s2 <= f);
    }
#endif
    auto cnt = ed - (start + 1);
    n += cnt;
  }
  MSGLN(n);
}