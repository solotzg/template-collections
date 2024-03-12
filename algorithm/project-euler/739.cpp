#include <utils/utils.h>

#ifdef NDEBUG
constexpr size_t m = 1e8;
#else
constexpr size_t m = 20;
#endif

constexpr size_t N = 2 * m + 1;
constexpr I32 MOD = 1e9 + 7;
using Factorial = utils::Factorial<N, MOD>;

// https://mathworld.wolfram.com/CatalansTriangle.html
I32 CatalanTriangle(size_t n, size_t k, const Factorial &fac) {
  using MulOp = utils::ModuloOperator<I32, I64, MOD>;
  auto p = MulOp::mul(fac.val(n + k), (n - k + 1));
  p = MulOp::mul(p, fac.inv(k));
  p = MulOp::mul(p, fac.inv(n + 1));
  return p;
}

I32 Solve(size_t n) {
  using Op = utils::ModuloOperator<I32, I64, MOD>;

  Factorial *fac = new Factorial();

  I32 a = 1, b = 3;
  I32 res = 0;
  rep(i, 1, n) {
    auto o = CatalanTriangle(n - 2, n - 1 - i, *fac);
    DEBUG_FMSGLN("+ {} * {}", o, b);

    o = Op::mul(o, b);
    res = Op::add(res, o);

    auto c = Op::add(a, b);
    a = b;
    b = c;
  }

  return res;
}

void ShowCatalanTriangle(size_t maxn, const Factorial &fac) {
  rep(n, 0, maxn) {
    repd(k, 0, n) {
      auto y = CatalanTriangle(n, k, fac);
      FMSG("{:>10} ", y);
    }
    MSGLN();
  }
}

int main() {
  SHOW_TIME_COST();

  Factorial *fac = new Factorial();
  DEBUG_SCOPE(ShowCatalanTriangle(m, *fac));
  MSGLN(Solve(m));

  return 0;
}