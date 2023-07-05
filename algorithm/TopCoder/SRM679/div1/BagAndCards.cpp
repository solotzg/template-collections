#include <bits/stdc++.h>
using namespace std;
struct BagAndCards {
  const static int N = 505, MOD = 1e9 + 7;
  typedef long long ll;
  int add(int x, int y) { return (x += y) >= MOD ? x - MOD : x; }
  int mul(int x, int y) { return (ll)x * y % MOD; }
  int getHash(int n, int m, int x, int a, int b, int c, string isGood) {
    static int dp[N][N], count[N][N];
    for (int i = 0; i <= n - 1; ++i) {
      for (int j = 0; j < m; ++j) {
        count[i][j] = x;
        x = (((ll)x * a + b) ^ (ll)c) % MOD;
      }
    }
    memset(dp, 0, sizeof dp);
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < m; ++j) {
        for (int k = 0; k < m; ++k) {
          if (isGood[k + j] == 'Y')
            dp[i][k] = add(dp[i][k], count[i][j]);
        }
      }
    }
    int res = 0;
    for (int i = 0; i < n; ++i) {
      for (int j = i + 1; j < n; ++j) {
        int ans = 0;
        for (int k = 0; k < m; ++k)
          ans = add(ans, mul(count[i][k], dp[j][k]));
        res ^= ans;
      }
    }
    return res;
  }
};
