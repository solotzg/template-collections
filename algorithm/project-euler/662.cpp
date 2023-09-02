#include "utils/utils.h"

int main() {
  SHOW_TIME_COST();

  U64 maxx = 3, maxy = 4;
  maxx = 10, maxy = 10;
  maxx = 10000, maxy = 10000;
  U64 maxn = std::sqrt(maxx * maxx + maxy * maxy);
  std::vector<std::pair<U32, U32>> data;
  {
    std::vector<U64> fibos;
    for (U64 a = 1, b = 1;;) {
      fibos.emplace_back(b);
      // MSGLN(b);
      auto na = b, nb = a + b;
      a = na;
      b = nb;
      if (b > maxn)
        break;
    }

    for (auto fibo : fibos) {
      repd(x, 0, std::min(maxx, fibo)) {
        U64 x2 = x * x;
        U64 y2 = fibo * fibo - x2;
        U64 y = std::sqrt(y2);
        if (y * y == y2) {
          data.emplace_back(x, y);
        }
      }
    }

    std::sort(data.begin(), data.end());
  }

  std::vector<std::array<U32, 10001>> dp;
  dp.resize(maxx + 1, {});
  dp[0][0] = 1;
  repd(x, 0, maxx) {
    repd(y, 0, maxy) {
      for (auto &&[i, j] : data) {
        auto xx = i + x, yy = j + y;
        if (xx <= maxx && yy <= maxy) {
          auto &t = dp[xx][yy];
          t += dp[x][y];
          t %= U32(1e9 + 7);
        }
      }
    }
  }
  MSGLN(dp[maxx][maxy]);

  return 0;
}