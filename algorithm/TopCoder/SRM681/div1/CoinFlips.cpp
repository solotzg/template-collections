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

struct CoinFlips {
  const static int N = 305;
  int n;
  double p[N], sp[N];
  double dp[N][N][N];
  double getExpectation(vector<int> val, int prob) {
    n = val.size();
    double pr = prob / 1e9;
    mst(dp, 0);
    mst(sp, 0);
    repd(m, 3, n) {
      p[1] = pr;
      repd(i, 2, m) p[i] = p[i - 1] * (1.0 - pr);
      p[1] += pow(1.0 - pr, m);
      repd(i, 1, m) sp[i] = sp[i - 1] + p[i];
      repd(i, 1, m) repd(j, i + 2, m) {
        dp[m][i][j] += dp[m - 1][i][j] * (sp[m] - sp[j]);
        dp[m][i][j] += dp[m - 1][i][j - 1] * (sp[j - 1] - sp[i]);
        dp[m][i][j] += dp[m - 1][i - 1][j - 1] * (sp[i - 1] - sp[0]);
      }
      repd(i, 1, m - 2) dp[m][i][i + 2] += p[i + 1];
    }
    ///
    double res = 0.0;
    rp(i, n) rep(j, i + 2, n) res += val[i] * val[j] * dp[n][i + 1][j + 1];
    return res;
  }
};
int main() {
  CoinFlips *c = new CoinFlips;
  cout << c->getExpectation({5, 1, 4, 2, 3}, 100000000) << endl;
  return 0;
}
