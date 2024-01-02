#include <utils/utils.h>

struct Solve {
  static constexpr int N = 10;
  using Data = std::array<int, N>;
  void dfs(int n) { return dfs(n, maxn); }
  void dfs(int n, const int remain) {
    if (n >= N) {
      if (remain)
        return;
      if (data[0] >= maxn) {
        ;
      } else {
        Data p{};
        p[0] = data[0];
        int j = 1;
        rep(i, 1, data.size()) {
          if (data[i]) {
            p[j++] = data[i];
          }
        }
        mmp[p] += 1;
      }
      return;
    }

    repd(i, 0, remain) {
      data[n] += i;
      dfs(n + 1, remain - i);
      data[n] -= i;
    }
  }

  struct Hash {
    size_t operator()(const Data &v) const _NOEXCEPT {
      return std::hash<std::string_view>()(std::string_view{
          (char *)(v.data()), v.size() * sizeof(Data::value_type)});
    }
  };

  Solve(int maxn) : maxn(maxn) {}

  const int maxn;
  Data data{};
  std::unordered_map<Data, U64, Hash> mmp;
};

U64 comb[13][13];

void init() {
  rp(i, 13) {
    comb[i][0] = comb[i][i] = 1;
    rep(j, 1, i) { comb[i][j] = comb[i - 1][j - 1] + comb[i - 1][j]; }
  }
}

U64 calc(const Solve::Data &d, int maxn) {
  U64 r = 1;
  rp(i, d.size()) {
    if (!d[i])
      continue;
    U64 x = comb[maxn][d[i]];
    maxn -= d[i];
    r *= x;
  }
  ASSERT_EQ(maxn, 0);
  return r;
}

U64 calc2(Solve::Data d, const int maxn) {
  U64 r = 0;
  if (!d[0]) {
    r += calc(d, maxn);
  } else {
    rep(i, 1, d.size()) {
      if (!d[i])
        continue;
      d[i] -= 1;
      r += calc(d, maxn - 1);
      d[i] += 1;
    }
  }
  return (r - 1) * (r) / 2;
}

int main() {
  init();
  const int maxn = 12;
  Solve s(maxn);
  s.dfs(0);
  ASSERT_EQ(
      std::pow(10, maxn) - 1,
      std::accumulate(s.mmp.begin(), s.mmp.end(), U64(0), [](auto s, auto &&o) {
        return s + calc(o.first, maxn) * o.second;
      }));

  ASSERT_EQ(calc2({1, 2, 1}, 4), 36);

  auto r =
      std::accumulate(s.mmp.begin(), s.mmp.end(), U64(0), [](auto s, auto &&o) {
        return s + calc2(o.first, maxn) * o.second;
      });
  MSGLN(r);
  return 0;
}