
#include <algorithm>
#include <cmath>
#include <sys/_types/_int64_t.h>
#include <unordered_map>
#include <vector>

struct Solve {
  int64_t solve() {
    int64_t a = 1, b = 1;
    int64_t mm = std::pow(10, 13);
    int64_t last = 0;
    for (;;) {
      if (a >= mm) {
        break;
      }
      fibo.emplace_back(a);
      sum_fibo.emplace_back(last + a);
      last = sum_fibo.back();
      auto c = a + b;
      b = a;
      a = c;
    }

    // mm = 100;

    int64_t ss = 0;
    for (int64_t m = mm; m <= mm; ++m) {
      auto k = dfs(m, fibo.size() - 1);
      MSG(k << " ");
      ss += k;
    }
    MSGLN("");

    return ss;
  }

  int64_t dfs(int64_t num, int64_t top) {
    if (num == 0) {
      return 1;
    }
    auto [bg, ed] = get_min(num);
    top = std::min(top, ed);

    if (auto it = data_.find({num, top}); it != data_.end())
      return it->second;

    int64_t res = 0;

    if (top < bg)
      return 0;

    if (num >= fibo[top])
      res += dfs(num - fibo[top], top - 1);
    if (top != 0)
      res += dfs(num, top - 1);

    return data_[{num, top}] = res;
  }

  std::pair<int64_t, int64_t> get_min(int64_t x) {
    if (auto it = num_cache_.find(x); it != num_cache_.end())
      return it->second;
    auto idx = std::lower_bound(sum_fibo.begin(), sum_fibo.end(), x) -
               sum_fibo.begin();
    auto top = std::upper_bound(fibo.begin(), fibo.end(), x) - fibo.begin() - 1;
    return num_cache_[x] = {idx, top};
  }

  std::map<std::pair<int64_t, int64_t>, int64_t> data_;
  std::unordered_map<int64_t, std::pair<int64_t, int64_t>> num_cache_;
  std::vector<int64_t> fibo;
  std::vector<int64_t> sum_fibo;
};

// https://anonshacker.com/url.php?q=aHR0cHM6Ly9wcm9qZWN0ZXVsZXIubmV0L3Byb2JsZW09NzU1
int main() {
  SHOW_TIME_COST;

  MSGLN(Solve{}.solve());
  return 0;
}