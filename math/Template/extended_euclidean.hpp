#include "utils/head_define.h"

static int64_t extend_gcd(int64_t a, int64_t b, int64_t &x, int64_t &y) {
  if (b == 0) {
    x = 1;
    y = 0;
    return a;
  } else {
    int64_t r = extend_gcd(b, a % b, x, y);
    int64_t t = x;
    x = y;
    y = t - (a / b) * y;
    return r;
  }
}
static int64_t inverse(int64_t a, int64_t mod) {
  int64_t x, y;
  auto gcd = extend_gcd(a, mod, x, y);
  assert(gcd == 1);
  x = (x % mod + mod) % mod;
  return x;
}

DECLARE_DEBUG_TEST_CODE(static int _test_extend_gcd() {
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
  return 0;
})