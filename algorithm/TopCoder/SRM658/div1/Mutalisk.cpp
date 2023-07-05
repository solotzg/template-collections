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

struct Mutalisk {
  int calc(VI &x, int up) {
    static int dp[22][145][145];
    const int inf = 0x3f3f3f3f;
    mst(dp, inf);
    dp[0][0][0] = 0;
    rp(i, x.size()) {
      int o = x[i];
      repd(a, 0, (o + 8) / 9) {
        repd(b, 0, (o + 2) / 3) {
          int c = max(0, o - 9 * a - 3 * b);
          if (a + b + c > up)
            continue;
          for (int q = 0; q + a <= up; ++q) {
            for (int p = 0; p + b <= up; ++p)
              if (dp[i][q][p] != inf) {
                dp[i + 1][q + a][p + b] =
                    min(dp[i + 1][q + a][p + b], dp[i][q][p] + c);
              }
          }
        }
      }
    }
    repd(i, 0, up) repd(j, 0, up) if (dp[x.size()][i][j] <= up) return 1;
    return 0;
  }
  int minimalAttacks(vector<int> x) {
    int mx = 0;
    for (int i : x)
      mx += (i + 8) / 9;
    int bg = 0, ed = mx;
    while (bg < ed) {
      int mid = (bg + ed) / 2;
      if (calc(x, mid))
        ed = mid;
      else
        bg = mid + 1;
    }
    return bg;
  }
};
