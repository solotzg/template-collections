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

struct Queueing {
  double dp[1005][1005];
  double probFirst(int len1, int len2, int p_1, int p_2) {
    double p1 = 1.0 / p_1, p2 = 1.0 / p_2;
    double q1 = 1.0 - p1, q2 = 1.0 - p2;
    dp[0][0] = 0.0;
    repd(i, 1, len2) dp[0][i] = 1.0;
    repd(i, 1, len1) dp[i][0] = 0.0;
    repd(i, 1, len1) {
      repd(j, 1, len2) {
        dp[i][j] = (p1 * q2 * dp[i - 1][j] + p2 * q1 * dp[i][j - 1] +
                    p1 * p2 * dp[i - 1][j - 1]) /
                   (1.0 - q1 * q2);
      }
    }
    return dp[len1][len2];
  }
};
