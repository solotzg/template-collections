#include "head_define.h"

const static int MOD = 1e9 + 7;
inline int mul(int a, int b) { return (LL)a * b % MOD; }
inline void smul(int &a, int b) { a = mul(a, b); }
inline int add(int a, int b) {
  return (a += b) >= MOD ? a - MOD : (a < 0 ? a + MOD : a);
}
inline void sadd(int &a, int b) { a = add(a, b); }

inline int fast_pow_mod(int a, int b) {
  int r = 1, p = a;
  while (b) {
    if (b & 1)
      r = mul(r, p);
    p = mul(p, p);
    b >>= 1;
  }
  return r;
}

template <size_t N> struct Comb {
  void init() {
    rp(i, N) {
      comb[i][0] = comb[i][i] = 1;
      rep(j, 1, i) { comb[i][j] = add(comb[i - 1][j - 1], comb[i - 1][j]); }
    }
  }
  int get_comb(int n, int k) {
    assert(k <= n);
    assert(n < N);
    assert(k < N);
    return comb[n][k];
  }

  int comb[N][N];
};

template <size_t N> struct Factorial {
  void init() {
    A[0] = RA[0] = 1;
    rep(i, 1, N) A[i] = mul(A[i - 1], i), RA[i] = pow(A[i], MOD - 2);
  }
  int get_comb(int n, int k) {
    assert(k <= n);
    assert(n < N);
    assert(k < N);
    return mul(A[n], mul(RA[k], RA[n - k]));
  }
  int A[N], RA[N];
};

template <class T, class U> T cast(U x) {
  T y;
  std::ostringstream a;
  a << x;
  std::istringstream b(a.str());
  b >> y;
  return y;
}

template <class T>
std::vector<T> split(const std::string &s, const std::string &x = " ") {
  std::vector<T> r;
  rp(i, s.size()) {
    std::string c;
    while (i < (int)s.size() && x.find(s[i]) == std::string::npos)
      c += s[i++];
    if (c.size())
      r.push_back(cast<T>(c));
  }
  return r;
}

template <class T> inline void tin(T &x) {
  int sg = 1;
  char c;
  while (((c = getchar()) < '0' || c > '9') && c != '-')
    ;
  c == '-' ? (sg = -1, x = 0) : (x = c - '0');
  while ((c = getchar()) >= '0' && c <= '9')
    x = x * 10 + c - '0';
  x *= sg;
}
