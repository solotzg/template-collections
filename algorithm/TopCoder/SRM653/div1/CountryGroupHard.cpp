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

struct CountryGroupHard {
  int dp[200];
  int go(VI &a) {
    int n = a.size();
    mst(dp, 0);
    dp[0] = 1;
    rp(i, n) {
      rep(j, i, n) {
        int k = j - i + 1;
        int f = 1;
        repd(o, i, j) if (a[o] && a[o] != k) {
          f = 0;
          break;
        }
        if (!f)
          continue;
        dp[j + 1] += dp[i];
        if (dp[j + 1] > 2)
          dp[i + 1] = 2;
      }
    }
    return dp[n] == 1;
  }
  string solve(vector<int> a) {
    if (go(a))
      return "Sufficient";
    return "Insufficient";
  }
};
