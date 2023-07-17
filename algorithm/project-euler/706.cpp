#include "utils/utils.h"

constexpr U32 mod = 1e9 + 7;

using OperatorWithModulo = utils::OperatorWithModulo<U32, U64>;

struct Num {
  using T = U32;
  Num(T data = {}) : data_(data) {}
  Num &operator+=(const Num &src) {
    data_ = OperatorWithModulo::add_mod(data_, src.data_, mod);
    return *this;
  }
  Num operator*(const Num &src) const {
    return Num{OperatorWithModulo::mul_mod(data_, src.data_, mod)};
  }
  T data() const { return data_; }

private:
  T data_;
};

struct Node {
  Num data[3][3][3][3][3][3];
  auto &operator[](size_t i) { return data[i]; }
};

Num F(size_t n) {
  Num res = 0;
  std::vector<Node> dp(2);
  int cnt[] = {3, 3, 3};
  rp(i, 3) {
    std::memset(dp.data(), 0, dp.size() * sizeof(Node));
    int cur = 0;
    int pre = 1 - cur;
    dp[pre][i == 0][i == 1][i == 2][i == 0][i == 1][i == 2] = 1;
    repd(d, 2, n) {
      std::memset(&dp[cur], 0, sizeof(Node));
      rp(a, 3) rp(b, 3) rp(c, 3) rp(x, 3) rp(y, 3) rp(z, 3) {
        // 0,3,6,9
        dp[cur][(a + x + 1) % 3][(b + y) % 3][(c + z) % 3][(x + 1) % 3][y][z] +=
            dp[pre][a][b][c][x][y][z] * 4;
        // 1,4,7
        dp[cur][(a + z) % 3][(b + x + 1) % 3][(c + y) % 3][z][(x + 1) % 3][y] +=
            dp[pre][a][b][c][x][y][z] * 3;
        // 2,5,8
        dp[cur][(a + y) % 3][(b + z) % 3][(c + x + 1) % 3][y][z][(x + 1) % 3] +=
            dp[pre][a][b][c][x][y][z] * 3;
      }
      std::swap(cur, pre);
    }
    Num r = 0;
    rp(b, 3) rp(c, 3) rp(x, 3) rp(y, 3) rp(z, 3) {
      r += dp[pre][0][b][c][x][y][z];
    }
    res += r * cnt[i];
  }

  return res;
}

int main() {
  SHOW_TIME_COST();
  DEBUG_SCOPE({ ASSERT_EQ(F(6).data(), 290898); });

  RUNTIME_ASSERT_EQ(884837055, F(1e5).data());
  return 0;
}