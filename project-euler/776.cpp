#include "utils/utils.h"

void split_digits(int64_t n, int *nums, int &len) {
  len = 0;
  for (auto x = n; x;) {
    nums[len++] = x % 10;
    x /= 10;
  }
  std::reverse(nums, nums + len);
}

struct Test {
  void init() {
    memset(cnt, 0, sizeof(cnt));
    memset(val, 0, sizeof(val));

    cnt[0][0] = 1;

    rep(i, 1, 20) {
      rp(j, 200) {
        rp(x, 10) {
          if (j - x >= 0) {
            cnt[i][j] += cnt[i - 1][j - x];
            val[i][j] += val[i - 1][j - x] * 10 + cnt[i - 1][j - x] * x;
          }
        }
      }
    }
  }

  double calc(int64_t n) {
    int nums[20];
    int len = 0;
    split_digits(n, nums, len);
    double res = double(n) / std::accumulate(nums, nums + len, 0);
    for (int64_t index = len - 1, tens = 1; index >= 0; index--, tens *= 10) {
      if (nums[index] == 0)
        continue;
      nums[index]--;
      for (; nums[index] >= 0; nums[index]--) {
        auto prefix_digits_sum = std::accumulate(nums, nums + index + 1, 0);
        double prefix = 0;
        rp(i, index + 1) { prefix = prefix * 10 + nums[i]; }
        prefix *= tens;
        auto remain_cnt = len - 1 - index;
        rep(x, 1, 200) {
          if (x - prefix_digits_sum < 0 ||
              0 == cnt[remain_cnt][x - prefix_digits_sum])
            continue;
          auto d = prefix / x * cnt[remain_cnt][x - prefix_digits_sum] +
                   val[remain_cnt][x - prefix_digits_sum] / x;
          res += d;
        }
      }
    }
    return res;
  }

  double cnt[20][200];
  double val[20][200];
};

double Solve(int64_t n) {
  int nums[20];
  int len = 0;

  double res = 0.0;
  repd(i, 1, n) {
    split_digits(i, nums, len);
    auto prefix_digits_sum = std::accumulate(nums, nums + len, 0);
    res += double(i) / prefix_digits_sum;
  }
  return res;
}

int main() {
  SHOW_TIME_COST();
  Test t;
  t.init();
  int64_t n = 1234567890123456789;
  auto r = t.calc(n);
  FMSGLN("{:.12e}", r);
  ASSERT_EQ(r, Solve(n));
}