template <int SIZE> struct Test {
  int run_0(int index) {
    auto &s = data[index].first;
    auto &&check_0 = [&]() {
      rp(i, s.size()) {
        if (mmp[i][s[i]] == TRUE)
          return false;
      }
      return true;
    };
    if (!check_0())
      return 0;
    std::array<std::tuple<int, int, int>, SIZE> ori;
    save(ori, index);
    if (dfs(index + 1))
      return 1;
    for (auto &&[a, b, c] : ori) {
      mmp[a][b] = c;
    }
    return 0;
  }
  int run_1(int index) {
    auto &s = data[index].first;
    auto &&check_1 = [&](int t) {
      if (mmp[t][s[t]] == FALSE)
        return false;
      if (res[t] != s[t] && res[t] != UNKNOWN)
        return false;
      rp(i, s.size()) {
        if (i != t) {
          if (mmp[i][s[i]] == TRUE)
            return false;
        }
      }
      return true;
    };
    rp(i, s.size()) {
      if (!check_1(i))
        continue;
      std::array<std::tuple<int, int, int>, SIZE> ori;
      int ori_res = res[i];
      save(ori, index);
      {
        mmp[i][s[i]] = TRUE;
        res[i] = s[i];
      }
      if (dfs(index + 1))
        return 1;
      for (auto &&[a, b, c] : ori) {
        mmp[a][b] = c;
      }
      { res[i] = ori_res; }
    }
    return 0;
  }
  void save(std::array<std::tuple<int, int, int>, SIZE> &ori, int index) {
    auto &s = data[index].first;
    rp(i, s.size()) {
      ori[i] = {i, s[i], mmp[i][s[i]]};
      mmp[i][s[i]] = FALSE;
    }
  }

  int run_2(int index) {
    auto &s = data[index].first;
    auto &&check_2 = [&](int a, int b) {
      if (mmp[a][s[a]] == FALSE)
        return false;
      if (mmp[b][s[b]] == FALSE)
        return false;
      if (res[a] != s[a] && res[a] != UNKNOWN)
        return false;
      if (res[b] != s[b] && res[b] != UNKNOWN)
        return false;
      rp(i, s.size()) {
        if (i != a && i != b) {
          if (mmp[i][s[i]] == TRUE)
            return false;
        }
      }
      return true;
    };

    rp(i, s.size()) rep(j, i + 1, s.size()) {
      if (!check_2(i, j))
        continue;
      std::array<std::tuple<int, int, int>, SIZE> ori;
      int ori_res[2] = {res[i], res[j]};
      save(ori, index);
      {
        mmp[i][s[i]] = TRUE;
        res[i] = s[i];
        mmp[j][s[j]] = TRUE;
        res[j] = s[j];
      }
      if (dfs(index + 1))
        return 1;
      for (auto &&[a, b, c] : ori) {
        mmp[a][b] = c;
      }
      {
        res[i] = ori_res[0];
        res[j] = ori_res[1];
      }
    }
    return 0;
  }
  int run_3(int index) {
    auto &s = data[index].first;
    auto &&check_3 = [&](int a, int b, int c) {
      if (mmp[a][s[a]] == FALSE)
        return false;
      if (mmp[b][s[b]] == FALSE)
        return false;
      if (mmp[c][s[c]] == FALSE)
        return false;
      if (res[a] != s[a] && res[a] != UNKNOWN)
        return false;
      if (res[b] != s[b] && res[b] != UNKNOWN)
        return false;
      if (res[c] != s[c] && res[c] != UNKNOWN)
        return false;
      rp(i, s.size()) {
        if (i != a && i != b && i != c) {
          if (mmp[i][s[i]] == TRUE)
            return false;
        }
      }
      return true;
    };

    rp(i, s.size()) rep(j, i + 1, s.size()) rep(k, j + 1, s.size()) {
      if (!check_3(i, j, k))
        continue;
      std::array<std::tuple<int, int, int>, SIZE> ori;
      int ori_res[3] = {res[i], res[j], res[k]};
      save(ori, index);
      {
        mmp[i][s[i]] = TRUE;
        res[i] = s[i];
        mmp[j][s[j]] = TRUE;
        res[j] = s[j];
        mmp[k][s[k]] = TRUE;
        res[k] = s[k];
      }
      if (dfs(index + 1))
        return 1;
      for (auto &&[a, b, c] : ori) {
        mmp[a][b] = c;
      }
      {
        res[i] = ori_res[0];
        res[j] = ori_res[1];
        res[k] = ori_res[2];
      }
    }
    return 0;
  }
  int dfs(int index) {
    if (index == data.size())
      return 1;
    switch (data[index].second) {
    case 0: {
      if (run_0(index))
        return 1;
      break;
    }
    case 1: {
      if (run_1(index))
        return 1;

      break;
    }
    case 2: {
      if (run_2(index))
        return 1;

      break;
    }
    case 3: {
      if (run_3(index))
        return 1;

      break;
    }
    }
    return 0;
  }
  void run(std::vector<std::pair<std::array<int, SIZE>, int>> &kk) {
    data = kk;
    std::sort(data.begin(), data.end(),
              [](const auto &x, const auto &y) { return x.second < y.second; });

    std::for_each(mmp.begin(), mmp.end(), [](auto &e) { e.fill(UNKNOWN); });
    res.fill(UNKNOWN);

    if (!dfs(0))
      std::terminate();

    rp(i, res.size()) {
      auto c = res[i];
      if (c == UNKNOWN) {
        auto ok = std::any_of(mmp[i].begin(), mmp[i].end(), [&](const auto &e) {
          if (e != FALSE) {
            c = &e - &mmp[i][0];
            return 1;
          }
          return 0;
        });
        assert(ok);
      }
      MSG(c);
    }
    MSGLN("");
  }
  static constexpr int UNKNOWN = -1;
  static constexpr int TRUE = 1;
  static constexpr int FALSE = 2;
  std::vector<std::pair<std::array<int, SIZE>, int>> data;
  std::array<std::array<int, 10>, SIZE> mmp;
  std::array<int, SIZE> res;
};

template <int size> void go() {}
template <> void go<16>() {
  std::vector<std::pair<std::array<int, 16>, int>> data = {
      {{5, 6, 1, 6, 1, 8, 5, 6, 5, 0, 5, 1, 8, 2, 9, 3}, 2},
      {{3, 8, 4, 7, 4, 3, 9, 6, 4, 7, 2, 9, 3, 0, 4, 7}, 1},
      {{5, 8, 5, 5, 4, 6, 2, 9, 4, 0, 8, 1, 0, 5, 8, 7}, 3},
      {{9, 7, 4, 2, 8, 5, 5, 5, 0, 7, 0, 6, 8, 3, 5, 3}, 3},
      {{4, 2, 9, 6, 8, 4, 9, 6, 4, 3, 6, 0, 7, 5, 4, 3}, 3},
      {{3, 1, 7, 4, 2, 4, 8, 4, 3, 9, 4, 6, 5, 8, 5, 8}, 1},
      {{4, 5, 1, 3, 5, 5, 9, 0, 9, 4, 1, 4, 6, 1, 1, 7}, 2},
      {{7, 8, 9, 0, 9, 7, 1, 5, 4, 8, 9, 0, 8, 0, 6, 7}, 3},
      {{8, 1, 5, 7, 3, 5, 6, 3, 4, 4, 1, 1, 8, 4, 8, 3}, 1},
      {{2, 6, 1, 5, 2, 5, 0, 7, 4, 4, 3, 8, 6, 8, 9, 9}, 2},
      {{8, 6, 9, 0, 0, 9, 5, 8, 5, 1, 5, 2, 6, 2, 5, 4}, 3},
      {{6, 3, 7, 5, 7, 1, 1, 9, 1, 5, 0, 7, 7, 0, 5, 0}, 1},
      {{6, 9, 1, 3, 8, 5, 9, 1, 7, 3, 1, 2, 1, 3, 6, 0}, 1},
      {{6, 4, 4, 2, 8, 8, 9, 0, 5, 5, 0, 4, 2, 7, 6, 8}, 2},
      {{2, 3, 2, 1, 3, 8, 6, 1, 0, 4, 3, 0, 3, 8, 4, 5}, 0},
      {{2, 3, 2, 6, 5, 0, 9, 4, 7, 1, 2, 7, 1, 4, 4, 8}, 2},
      {{5, 2, 5, 1, 5, 8, 3, 3, 7, 9, 6, 4, 4, 3, 2, 2}, 2},
      {{1, 7, 4, 8, 2, 7, 0, 4, 7, 6, 7, 5, 8, 2, 7, 6}, 3},
      {{4, 8, 9, 5, 7, 2, 2, 6, 5, 2, 1, 9, 0, 3, 0, 6}, 1},
      {{3, 0, 4, 1, 6, 3, 1, 1, 1, 7, 2, 2, 4, 6, 3, 5}, 3},
      {{1, 8, 4, 1, 2, 3, 6, 4, 5, 4, 3, 2, 4, 5, 8, 9}, 3},
      {{2, 6, 5, 9, 8, 6, 2, 6, 3, 7, 3, 1, 6, 8, 6, 7}, 2}};
  ;
  Test<16> t;
  t.run(data);
}
template <> void go<5>() {
  std::vector<std::pair<std::array<int, 5>, int>> data = {
      {{9, 0, 3, 4, 2}, 2}, {{7, 0, 7, 9, 4}, 0}, {{3, 9, 4, 5, 8}, 2},
      {{3, 4, 1, 0, 9}, 1}, {{5, 1, 5, 4, 5}, 2}, {{1, 2, 5, 3, 1}, 1}};

  Test<5> t;
  t.run(data);
}

int main() {
  SHOW_TIME_COST()
  go<16>();
}
