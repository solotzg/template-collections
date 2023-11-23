#include "algorithm/data_structure/Template/matrix_operation.hpp"
#include "algorithm/math/Template/euler_function.hpp"
#include "algorithm/math/Template/extended_euclidean.hpp"
#include "algorithm/math/Template/prime_factors.hpp"
#include "algorithm/math/miller_rabin/miller_rabin.hpp"
#include "tests/tests.h"

namespace tests {

static void _test_prime_factors() {
  typedef uint64_t T;
  T a = 20100224546;
  auto prime = PrimeHelper::GenPrimeHelperWithMaxNum(1e9);
  assert(!PrimeHelper::IsPrimeBruceForce(a));
  {
    auto factors = prime.Decompose(a);
    T sum = 1;
    for (const auto &[k, v] : factors) {
      sum *= utils::fast_pow(k, v);
      assert(PrimeHelper::IsPrimeBruceForce(k));
    }
    assert(sum == a);
  }
  {
    auto factors = prime.Decompose(240);
    size_t cnt = 0;
    factors.dfs([&](T p) { cnt += 1; });
    assert(cnt == 20);
  }
  {
    PrimeHelper::FactorMap factors;
    prime.Decompose(a, [&](int64_t x, size_t cnt) { factors.add(x, cnt); });
    T sum = 1;
    for (const auto &[k, v] : factors.data()) {
      sum *= utils::fast_pow(k, v);
      assert(PrimeHelper::IsPrimeBruceForce(k));
    }
    assert(sum == a);
  }
  {
    PrimeHelper::FactorMap factors = prime.Decompose(a);
    T sum = 1;
    for (const auto &[k, v] : factors.data()) {
      sum *= utils::fast_pow(k, v);
      assert(PrimeHelper::IsPrimeBruceForce(k));
    }
    assert(sum == a);
  }

  a = 1;
  for (int i = 0; i < prime.primes().size() && a < 1e10;
       a *= prime.primes()[i++])
    ;
  {
    auto factors = prime.Decompose(a);
    T sum = 1;
    for (const auto &[k, v] : factors) {
      sum *= utils::fast_pow(k, v);
    }
    assert(sum == a);
  }
  {
    int x = 656744;
    PrimeHelper::FactorMap factors;
    PrimeForDivide p(x);
    p.Decompose(x, [&](int f, int c) { factors.add(f, c); });
    int sum = 1;
    for (const auto &[k, v] : factors.data()) {
      sum *= utils::fast_pow(k, v);
    }
    assert(sum == x);
  }
  {
    PrimeForDivide p(2099);
    p.Decompose(2099, [](int f, int c) {
      ASSERT_EQ(f, 2099);
      ASSERT_EQ(c, 1);
    });
    ASSERT(p.IsPrime(2099));
  }
  prime.InitPiSmall();
  { ASSERT_EQ(prime.Pi(1e9), 50847534); }
}
} // namespace tests

namespace tests {
static void _test_extend_gcd() {
  // http://www.cnblogs.com/frog112111/archive/2012/08/19/2646012.html
  // http://www.cnblogs.com/void/archive/2011/04/18/2020357.html
  {
    // inverse
    assert(inverse(2, 5) == 3);
  }
  {
    // gcd
    int64_t a = 20, b = 28, x, y;
    int64_t r = extend_gcd(a, b, x, y);
    assert(r == 4);
  }
  {
    /*
    ax+by=c;
    c % gcd(a,b) == 0;
    */
    int64_t a = 4, b = 6, c = 8, x, y;
    int64_t r = extend_gcd(a, b, x, y);
    assert(a * (x * c / r) + b * (y * c / r) == c);
  }
}
} // namespace tests

namespace tests {
static void _test_euler_function() {
  EulerFunction euler_function{20};
  assert((euler_function.prime() == std::vector{2, 3, 5, 7, 11, 13, 17, 19}));
  for (auto &&p : euler_function.prime()) {
    ASSERT_EQ(euler_function.phi(p), p - 1);
  }
}
} // namespace tests

namespace tests {
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

  int t = 500;
  while (t--) {
    auto num = random() % int64_t(1e9) + 1;
    if (m.IsPrime(num)) {
      ASSERT(is_prime(num), num);
    } else {
      ASSERT(!is_prime(num));
      auto p = ({
        int64_t p = 1;
        for (auto &&x : m.FindFacs(num)) {
          p *= x;
        }
        p;
      });
      ASSERT_EQ(num, p);
    }
  }

  {
    auto n = m.FindFacs(3040419601);
    ASSERT_EQ(n.size(), 1);
    ASSERT_EQ(n.back(), 3040419601);
  }
}
} // namespace tests

namespace tests {
static void _test_matrix() {
  constexpr uint64_t MOD = 1e9 + 7;
  using M = Matrix<uint64_t, 2, 2>;
  uint64_t a = 0, b = 1, n = 0;
  for (; n < 1000; n++) {
    ASSERT(a == M::Fibonacci<U128>(n, MOD));
    auto c = (a + b) % MOD;
    a = b;
    b = c;
  }
}
} // namespace tests

namespace tests {
static void _test_algorithm() {
  _test_matrix();
  _test_miller_rabin();
  _test_euler_function();
  _test_extend_gcd();
  _test_prime_factors();
}
} // namespace tests

FUNC_FACTORY_REGISTER("algorithm", tests::_test_algorithm);
