// http://www.51nod.com/onlineJudge/questionCode.html#!problemId=1371
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
#define rep(i, a, b) for (int i(a), __tzg##i(b); i < __tzg##i; ++i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, 1 + b)
#define urp(i, a, b) for (int i(a), __tzg##i(b); i >= __tzg##i; --i)
#define vrp(i, v) for (auto i(v.begin()); i != (v).end(); ++i)
typedef long long ll;
struct Main {
  const static int N = 205;
  const int mod = 100000007;
  int dp[2][N][N];
  int comb[N][N];
  int n;
  int add(int a, int b) {
    a += b;
    if (a >= mod)
      a -= mod;
    return a;
  }
  int mul(int a, int b) { return (ll)a * b % mod; }
  void init() {
    rp(i, N) {
      comb[i][0] = comb[i][i] = 1;
      rep(j, 1, i) { comb[i][j] = add(comb[i - 1][j - 1], comb[i - 1][j]); }
    }
  }
  void solve() {
    memset(dp, 0, sizeof dp);
    init();
    cin >> n;
    int cur = 0, pre = 1;
    dp[cur][0][0] = 1;
    repd(i, 0, n - 1) {
      cur = pre;
      pre = 1 - pre;
      memset(dp[cur], 0, sizeof dp[cur]);
      repd(a, 0, i) {
        repd(b, 0, i - a) {
          int o = dp[pre][a][b];
          if (a >= 1) {
            dp[cur][a][b] = add(dp[cur][a][b], mul(o, a));
          }
          dp[cur][a][b] = add(dp[cur][a][b], o);
          if (a >= 2) {
            dp[cur][a - 1][b + 2] =
                add(dp[cur][a - 1][b + 2], mul(o, comb[a][2]));
          }
          if (b >= 2) {
            dp[cur][a + 1][b - 2] =
                add(dp[cur][a + 1][b - 2], mul(o, comb[b][2]));
          }
          if (a >= 1) {
            dp[cur][a - 1][b + 2] = add(dp[cur][a - 1][b + 2], mul(o, a));
          }
          if (b >= 1) {
            dp[cur][a][b] = add(dp[cur][a][b], mul(o, b));
          }
          if (a >= 1 && b >= 1) {
            dp[cur][a][b] = add(dp[cur][a][b], mul(o, mul(a, b)));
          }
          if (a >= 1)
            dp[cur][a][b + 1] = add(dp[cur][a][b + 1], mul(o, a));
          if (b >= 1)
            dp[cur][a + 1][b - 1] = add(dp[cur][a + 1][b - 1], mul(o, b));
          dp[cur][a][b + 1] = add(dp[cur][a][b + 1], o);
          dp[cur][a + 1][b] = add(dp[cur][a + 1][b], o);
        }
      }
    }
    int res = 0;
    repd(a, 0, n) {
      repd(b, 0, n - a) { res = add(res, dp[cur][a][b]); }
    }
    cout << res << endl;
  }
} _main;
int main() {
  _main.solve();
  return 0;
}
