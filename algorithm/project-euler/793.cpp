#include <utils/utils.h>

void foo(size_t n) {
  VLL data;
  data.reserve(n);
  data.emplace_back(290797);
  rep(i, 1, n) { data.emplace_back(data[i - 1] * data[i - 1] % 50515093); }
  std::sort(data.begin(), data.end());
  VLL res;
  size_t m = n * (n - 1) / 2;
  res.reserve(m);
  rp(i, n) {
    rep(j, i + 1, n) {
      auto p = data[i] * data[j];
      res.emplace_back(p);
    }
  }
  std::sort(res.begin(), res.end());
  std::nth_element(res.begin(), res.begin() + m / 2, res.end());
  MSGLN(n, ':', res[m / 2]);
}

size_t calc_fit_cnt(const VLL &data, LL prod) {
  // count(production < prod)
  size_t cnt = 0;
  rp(i, data.size()) {
    size_t bg = i + 1, ed = data.size();
    for (; bg != ed;) {
      auto m = (bg + ed) / 2;
      if (data[m] * data[i] < prod) {
        bg = m + 1;
      } else {
        ed = m;
      }
    }
    ASSERT_EQ(bg, ed);
    if (bg - (i + 1) > 0) {
      if (bg < data.size()) {
        ASSERT_LT(data[i] * data[bg - 1], prod);
        ASSERT_GE(data[i] * data[bg], prod);
      }
    }
    cnt += bg - (i + 1);
  }
  return cnt;
}

bool fit(const VLL &data, LL prod, size_t m) {
  size_t cnt = calc_fit_cnt(data, prod);
  // count(production < prod) <= m/2
  auto res = cnt <= (m / 2);
  return res;
}

void foo_optimized(size_t n) {
  size_t m = n * (n - 1) / 2;
  VLL data;
  data.reserve(n);
  data.emplace_back(290797);
  rep(i, 1, n) { data.emplace_back(data[i - 1] * data[i - 1] % 50515093); }
  std::sort(data.begin(), data.end());
  auto max_data = data[n - 2] * data[n - 1];
  auto min_data = data[0] * data[1];
  auto bg = min_data, ed = max_data + 1;
  for (; bg != ed;) {
    auto mid = (bg + ed) / 2;
    if (fit(data, mid, m)) {
      bg = mid + 1;
    } else {
      ed = mid;
    }
  }
  ASSERT_EQ(bg, ed);
  ASSERT(!fit(data, bg, m));
  auto res = bg - 1;
  ASSERT(fit(data, res, m));
  MSGLN(n, ':', res);
}

int main() {
  SHOW_TIME_COST()
#define FOO foo_optimized
  FOO(3);
  FOO(103);
  FOO(1000003);
#undef FOO
  return 0;
}