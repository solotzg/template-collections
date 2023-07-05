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

struct CampLunch {
  const int MOD = 1e9 + 7;
  int dp[17][17][1 << 16];
  void add(int &a, int b) {
    a += b;
    if (a >= MOD)
      a -= MOD;
  }
  int count(int N, int M, vector<string> a) {
    mst(dp, 0);
    dp[0][0][0] = 1;
    rp(i, N) {
      rp(j, M) rp(k, 1 << M) {
        int s = a[i][j] - 'A', p = dp[i][j][k];
        if (k & (1 << s)) {
          add(dp[i][j + 1][k - (1 << s)], p);
        } else {
          add(dp[i][j + 1][k], p);
          add(dp[i][j + 1][k | (1 << s)], p);
          if (j + 1 < M) {
            int o = a[i][j + 1] - 'A';
            if (_0(k & (1 << o)))
              add(dp[i][j + 2][k], p);
          }
        }
      }
      rp(j, 1 << M) dp[i + 1][0][j] = dp[i][M][j];
    }
    return dp[N][0][0];
  }
};