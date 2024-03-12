#include <utils/utils.h>

constexpr I32 mod = (1e9 + 7);

template <typename T = I32, typename Op = utils::DefaultModuloOperator>
struct BinaryIndexedTree {
  static int Lowbit(int x) { return x & -x; }

  static void Add(T *data, int pos, int val, int n) {
    for (int i = pos + 1; i <= n; i += Lowbit(i))
      Op::sadd(data[i - 1], val);
  }
  static void Add(std::vector<T> &data, int pos, int val) {
    return Add(data.data(), pos, val, data.size());
  }

  static T Sum(const T *data, int pos) {
    T res = 0;
    for (int i = pos + 1; i; i -= Lowbit(i))
      Op::sadd(res, data[i - 1]);
    return res;
  }

  static T Sum(const std::vector<T> &data, int pos) {
    return Sum(data.data(), pos);
  }
};

struct PE733 {
  using Op = utils::ModuloOperator<I32, I64, mod>;
  using BinaryIndexedTree = BinaryIndexedTree<I32, Op>;

  std::vector<I64> init(size_t n) {
    std::vector<I64> res;
    I64 p = 153;
    rp(i, n) {
      res.emplace_back(p);
      p = Op::mul_mod(p, 153, (1e7 + 19));
    }
    return res;
  }
  std::vector<I64> gen_order(const std::vector<I64> &v) {
    std::vector<I64> res(v.size());
    rp(i, v.size()) res[i] = i;
    std::sort(res.begin(), res.end(), [&v](int x, int y) {
      if (auto c = v[x] - v[y]; c == 0) {
        return x < y;
      } else {
        return c < 0;
      }
    });
    return res;
  }

  I64 Solve(int n) {
    auto data = init(n);
    auto seq = gen_order(data);

    constexpr int M = 4;
    std::vector<I32> dp[M + 1];
    std::vector<I32> num[M + 1];
    repd(i, 1, M) {
      dp[i].resize(n, 0);
      num[i].resize(n, 0);
    }
    {
      for (auto &&idx : seq) {
        auto d = data[idx];
        {
          BinaryIndexedTree::Add(dp[1], idx, d);
          BinaryIndexedTree::Add(num[1], idx, 1);
        }

        repd(k, 2, M) {
          I32 o = 0;

          auto m = BinaryIndexedTree::Sum(num[k - 1], idx - 1);
          o = Op::add(o, Op::mul(m, d));
          o = Op::add(o, BinaryIndexedTree::Sum(dp[k - 1], idx - 1));

          BinaryIndexedTree::Add(dp[k], idx, o);

          I32 p = BinaryIndexedTree::Sum(num[k - 1], idx - 1);
          BinaryIndexedTree::Add(num[k], idx, p);
        }
      }
    }

    I32 res = BinaryIndexedTree::Sum(dp[M], n - 1);

    return res;
  }
};

int main() {
  BinaryIndexedTree b;

  PE733 s;
  ASSERT_EQ(s.Solve(6), 94513710);
  ASSERT_EQ(s.Solve(100), 4465488724217 % mod);
  auto res = s.Solve(1e6);
  MSGLN(res);
  return 0;
}