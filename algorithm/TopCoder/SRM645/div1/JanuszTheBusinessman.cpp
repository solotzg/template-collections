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
const int N = 55 * 2;
struct _JanuszTheBusinessman {
  int dp[N];
  int n;
  PII p[N];
  int f(int dx) {
    if (dx == n)
      return 0;
    if (dp[dx] != -1)
      return dp[dx];
    int &res = dp[dx];
    res = 1e9;
    int l = p[dx].fi, r = p[dx].se;
    int nl, nr;
    rep(i, 0, n) {
      nl = p[i].fi, nr = p[i].se;
      if (nl > r || nr < l)
        continue;
      int pos = -1;
      rep(j, dx, n) {
        if (nl > p[j].se) {
          pos = -1;
          break;
        }
        if (p[j].fi > nr)
          break;
        else
          pos = j;
      }
      if (pos == -1)
        continue;
      res = min(res, 1 + f(pos + 1));
    }
    return res;
  }
  int makeGuestsReturn(vector<int> a, vector<int> d) {
    n = a.size();
    rp(i, n) p[i] = mp(a[i], d[i]);
    sort(p, p + n);
    mst(dp, -1);
    return f(0);
  }
};

struct JanuszTheBusinessman {
  const static int N = 365;
  int happy[N];
  int makeGuestsReturn(vector<int> a, vector<int> d) {
    int res = 0;
    set<int> s(vtr(d));
    mst(happy, 0);
    for (auto u : s) {
      int dx = -1;
      rp(i, a.size()) if (d[i] == u && !happy[i]) dx = i;
      if (dx == -1)
        continue;
      ++res;
      int opmital = -1;
      rp(i, a.size()) {
        int l = a[i], r = d[i];
        if (l <= u && r >= u) {
          opmital = opmital == -1 ? i : (r > d[opmital] ? i : opmital);
        }
      }
      rp(i, a.size()) {
        int l = a[i], r = d[i];
        if (a[opmital] > r || d[opmital] < l)
          continue;
        happy[i] = 1;
      }
    }
    return res;
  }
};
