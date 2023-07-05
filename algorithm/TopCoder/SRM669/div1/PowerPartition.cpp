#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
#define vtr(v) (v).begin(), (v).end()
#define mp(a, b) make_pair(a, b)
#define fi first
#define se second
#define pb(a) push_back(a)
#define _0(x) (!(x))
#define _1(x) (x)
typedef vector<int> VI;
typedef pair<int, int> PII;
typedef vector<PII> VPII;
typedef long long ll;
struct Matrix {
  typedef int T;
  const static int R = 70;
  const static int C = 70;
  const static T MOD = 1e9 + 7;
  T da[R][C];
  int r, c;
  Matrix(const T _da[][C], int _r, int _c) : r(_r), c(_c) {
    memcpy(da, _da, sizeof da);
  }
  Matrix(const Matrix &p) : r(p.r), c(p.c) { memcpy(da, p.da, sizeof da); }
  Matrix(const int _r, const int _c, int _ty = 0) : r(_r), c(_c) {
    memset(da, 0, sizeof da);
    if (_ty)
      for (int i = 0; i < r; ++i)
        da[i][i] = T(1);
  }
  Matrix operator*(const Matrix &a) const {
    Matrix res(r, a.c);
    for (int i = 0; i < res.r; ++i)
      for (int j = 0; j < res.c; ++j)
        for (int k = 0; k < c; ++k) {
          res.da[i][j] =
              ((long long)da[i][k] * a.da[k][j] + res.da[i][j]) % MOD;
        }
    return res;
  }
  const T *operator[](const int &i) const { return da[i]; }
  T *operator[](int i) { return da[i]; }
};
Matrix pow(const Matrix &a, int n) {
  Matrix res(a.r, a.c, 1), p = a;
  while (n) {
    if (n & 1)
      res = res * p;
    p = p * p;
    n >>= 1;
  }
  return res;
}
const static int MOD = 1e9 + 7;
struct PowerPartition {
  int count(int M, long long X) {
    VI nums;
    while (X) {
      nums.pb(X % M);
      X /= M;
    }
    int log = nums.size();
    vector<Matrix> pw;
    pw.pb(Matrix(1, 1, 1));
    rep(i, 1, log) {
      Matrix tmp = pow(pw.back(), M);
      tmp.r = tmp.c = i + 1;
      repd(j, 0, i) tmp[i][j] = 1;
      pw.pb(tmp);
    }
    Matrix cur(log, log, 1);
    urp(i, log - 1, 0) { cur = cur * pow(pw[i], nums[i]); }
    int ans = 0;
    urp(i, log - 1, 0) {
      if ((ans += cur[i][0]) >= MOD)
        ans -= MOD;
    }
    return ans;
  }
};
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif
  PowerPartition *p = new PowerPartition;
  assert(p->count(893, 1000000000000000000ll) == 262776600);
  return 0;
}
