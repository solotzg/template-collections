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
