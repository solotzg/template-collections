#include <utils/utils.h>

struct Solve {
  // static constexpr int N = 14;
  static constexpr int M = 6;
  using IDX = std::array<int, M>;
  using Value = double;

  struct Hash {
    size_t operator()(const IDX &key) const {
      return std::hash<std::string_view>()(
          {(const char *)(key.data()), key.size() * sizeof(IDX::value_type)});
    }
  };
  using Data = std::unordered_map<IDX, Value, Hash>;

  double dfs(const IDX idx, const int tol) {
    if (tol == 0) {
      return 0;
    }
    auto it = data.find(idx);
    if (it != data.end()) {
      return it->second;
    }
    it = data.emplace(idx, 0.0).first;
    auto &res = it->second;

    auto last = idx.back();
    rep(i, 1, M - 1) {
      int n = idx[i] * i;
      if (!n)
        continue;
      if (last == i) {
        double rate1 = 1.0 * i / tol;
        res += rate1;
      }
      double rate2 = (last == i) ? 1.0 * (n - i) / tol : 1.0 * n / tol;
      auto new_idx = idx;
      new_idx[i] -= 1;
      new_idx[i - 1] += 1;
      new_idx.back() = i - 1;
      res += rate2 * (1.0 + dfs(new_idx, tol - 1));
    }
    return res;
  }

  double solve() { return dfs({0, 0, 0, 0, 13, -1}, 52); }

  Data data;
};

int main() {
  Solve s;
  FMSGLN("{:.8f}", s.solve());
  return 0;
}