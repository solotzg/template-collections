#include "utils/head_define.h"
#include "utils/utils.h"
#include <sys/_types/_int64_t.h>

int64_t goo(int64_t x) {
  if (x % 2 == 0 || x % 3 == 0)
    return 0;
  int64_t a = 1, b = 1;
  int64_t n = 1;
  for (;;) {
    if (b == 0) {
      int64_t res = n, xa = a;
      for (;;) {
        if (xa == 1)
          return res;
        xa = xa * a % x;
        res += n;
      }
    }
    int64_t na = (a + 7 * b) % x;
    int64_t nb = (a + b) % x;
    a = na;
    b = nb;
    ++n;
  }
}

int main() {
  SHOW_TIME_COST();
  int64_t res = 0, n = 1e6;
  repd(x, 2, n) { res += goo(x); }

  MSGLN(res);
}