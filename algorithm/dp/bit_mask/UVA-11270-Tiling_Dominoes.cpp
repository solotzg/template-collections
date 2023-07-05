#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
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
ll dp[2][13][1 << 13];
ll mmp[105][105];
int main() {
#ifndef ONLINEE_JUDGE
  freopen("in.txt", "r", stdin);
#endif // ONLINEE_JUDGE
  int n, m;
  mst(mmp, -1);
  while (scanf("%d%d", &n, &m) != EOF) {
    if (m > n)
      swap(m, n);
    if (mmp[n][m] != -1) {
      cout << mmp[n][m] << endl;
      continue;
    }
    int cur = 0, nxt = 1;
    mst(dp, 0);
    dp[cur][0][0] = 1ll;
    rp(i, n) {
      rp(j, m) rp(k, 1 << m) {
        ll p = dp[cur][j][k];
        if (k & (1 << j)) {
          dp[cur][j + 1][k - (1 << j)] += p;
        } else {
          dp[cur][j + 1][k | (1 << j)] += p;
          if (j + 1 < m && _0(k & (1 << (j + 1)))) {
            dp[cur][j + 2][k] += p;
          }
        }
      }
      mst(dp[nxt], 0);
      rp(k, 1 << m) dp[nxt][0][k] += dp[cur][m][k];
      swap(cur, nxt);
    }
    mmp[n][m] = dp[cur][0][0];
    cout << mmp[n][m] << endl;
  }
  return 0;
}
