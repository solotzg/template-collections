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
struct BearFair2 {
  const static int N = 55, M = 1005;
  int dp[N][N][N];
  int cs[2][M];
  int go(int cn, int b, VI &up, VI &qu) {
    cs[0][0] = cs[1][0] = 0;
    rep(i, 1, M) cs[0][i] = cs[0][i - 1] + (i % 3 == 0),
                 cs[1][i] = cs[1][i - 1] + (i % 3 == 1);
    vector<PII> p;
    rp(i, up.size()) { p.pb(mp(up[i], qu[i])); }
    p.pb(mp(0, 0));
    p.pb(mp(b, cn));
    sort(vtr(p));
    mst(dp, 0);
    dp[0][0][0] = 1;
    int n = p.size();
    rep(i, 1, n) {
      int a = p[i].fi - p[i - 1].fi;
      int b = p[i].se - p[i - 1].se;
      if (a == 0) {
        if (b != 0)
          return 0;
        memcpy(dp[i], dp[i - 1], sizeof dp[i]);
        continue;
      }
      if (b < 0 || a < b)
        return 0;
      int t0 = cs[0][p[i].fi] - cs[0][p[i - 1].fi],
          t1 = cs[1][p[i].fi] - cs[1][p[i - 1].fi], t2 = a - t0 - t1;
      repd(c0, 0, t0) repd(c1, 0, t1) if (c0 + c1 <= b && b - c0 - c1 <= t2) {
        repd(p0, 0, cn) repd(p1, 0, cn) if (dp[i - 1][p0][p1])
            dp[i][p0 + c0][p1 + c1] = 1;
      }
    }
    return dp[n - 1][cn / 3][cn / 3];
  }
  string isFair(int n, int b, VI upTo, VI quantity) {
    if (go(n, b, upTo, quantity))
      return "fair";
    return "unfair";
  }
};
int main() { return 0; }
