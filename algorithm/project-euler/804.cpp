#include "utils/utils.h"

int64_t foo(int64_t n) {
  size_t res = 0;
  {
    // 41 * y*y <= n
    int64_t y = std::sqrt(n / 41.0);
    res += y * 2;
  }
  repd(x, 1, 1e16) {
    auto c = x * x;
    auto b = x;
    auto a = 41;
    // b * b < 4 * a * c
    c -= n;
    auto o = b * b - 4 * a * c;
    if (o >= 0) {
      auto f = std::sqrt(o);
      int64_t ed = floor((-b + f) / (2 * a));
      int64_t bg = ceil((-b - f) / (2 * a));
      res += ed - bg + 1;
    } else {
      break;
    }
  }
  rev_repd(x, -1, -1e16) {
    auto c = x * x;
    auto b = x;
    auto a = 41;
    // b * b < 4 * a * c
    c -= n;
    auto o = b * b - 4 * a * c;
    if (o >= 0) {
      auto f = std::sqrt(o);
      int64_t ed = floor((-b + f) / (2 * a));
      int64_t bg = ceil((-b - f) / (2 * a));
      res += ed - bg + 1;
    } else {
      break;
    }
  }
  return res;
}
int main() {
  int64_t n = 1e16;
  MSGLN(foo(n));
  return 0;
}