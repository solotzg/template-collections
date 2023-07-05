#include <bits/stdc++.h>
using namespace std;
struct BearCries {
  const int mod = 1000000007;
  static const int N = 205;
  int dp[N][N][N];
  int mul(int a, int b) { return (long long)(a)*b % mod; }
  void add(int &a, int b) {
    a += b;
    if (a >= mod)
      a -= mod;
  }
  int count(string message) {
    memset(dp, 0, sizeof dp);
    dp[0][0][0] = 1;
    int len = message.size();
    for (int i = 0; i < len; ++i)
      for (int j = 0; j <= len; ++j)
        for (int k = 0; k <= len; ++k)
          if (dp[i][j][k]) {
            if (message[i] == ';') {
              add(dp[i + 1][j + 1][k], dp[i][j][k]);
              if (k)
                add(dp[i + 1][j][k - 1], mul(dp[i][j][k], k));
            } else {
              if (j)
                add(dp[i + 1][j - 1][k + 1], mul(dp[i][j][k], j));
              add(dp[i + 1][j][k], mul(dp[i][j][k], k));
            }
          }
    return dp[len][0][0];
  }
};
