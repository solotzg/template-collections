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

const int INF = 0x3f3f3f3f;
struct SingingEasy {
  int dp[2005][2005][2];
  inline void sm(int &a, int b) { a = min(a, b); }
  int solve(VI p) {
    int n = p.size();
    mst(dp, INF);
    dp[0][0][0] = 0;
    dp[0][0][1] = 0;
    rp(i, n - 1) {
      repd(j, 0, i) {
        if (j == i) {
          sm(dp[i + 1][i + 1][0], dp[i][i][0] + abs(p[i + 1] - p[i]));
          sm(dp[i + 1][i + 1][1], dp[i][i][1] + abs(p[i + 1] - p[i]));
          sm(dp[i + 1][i][0], dp[i][i][1]);
          sm(dp[i + 1][i][1], dp[i][i][0]);
        } else {
          sm(dp[i + 1][j][0], dp[i][j][0] + abs(p[i + 1] - p[i]));
          sm(dp[i + 1][j][1], dp[i][j][1] + abs(p[i + 1] - p[i]));
          sm(dp[i + 1][i][0], dp[i][j][1] + abs(p[i + 1] - p[j]));
          sm(dp[i + 1][i][1], dp[i][j][0] + abs(p[i + 1] - p[j]));
        }
      }
    }
    int res = INF;
    rp(i, n) rp(j, 2) sm(res, dp[n - 1][i][j]);
    return res;
  }
};

int main() {
  SingingEasy *s = new SingingEasy;
  cout << s->solve({24, 13, 2,  4,  54, 23, 12, 53, 12, 23, 42, 13,
                    53, 12, 24, 12, 11, 24, 42, 52, 12, 32, 42})
       << endl;
  return 0;
}
