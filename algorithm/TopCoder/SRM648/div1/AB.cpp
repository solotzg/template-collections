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
#define bit(x, y) (((x) >> (y)) & 1)
typedef vector<int> VI;
typedef pair<int, int> PII;
typedef vector<PII> VPII;
typedef long long ll;

struct AB {
  string createString(int n, int k);
};

const int N = 55;
int dp[N][N][N * N];
string res[N][N][N * N];

int f(int n, int nb, int k) {
  if (k < 0 || nb < 0 || n < 0)
    return 0;
  if (nb > n)
    return 0;
  if ((n - nb) * nb < k)
    return 0;

  int &d = dp[n][nb][k];
  string &s = res[n][nb][k];
  if (d != -1)
    return d;
  d = 0;
  if (n == nb || nb == 0) {
    if (k != 0)
      d = 0, s = "";
    else {
      d = 1;
      s = "";
      if (n == nb) {
        rp(i, n) s += 'B';
      } else {
        rp(i, n) s += 'A';
      }
    }
    return d;
  }
  if (f(n - 1, nb, k - nb) == 1) {
    d = 1;
    s = "A" + res[n - 1][nb][k - nb];
    return d;
  }
  if (f(n - 1, nb - 1, k) == 1) {
    d = 1;
    s = "B" + res[n - 1][nb - 1][k];
    return d;
  }
  return d;
}

string AB::createString(int n, int k) {
  mst(dp, -1);
  repd(i, 0, n) if (f(n, i, k)) { return res[n][i][k]; }
  return "";
}
