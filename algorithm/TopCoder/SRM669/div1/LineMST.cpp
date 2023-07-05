#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
using namespace std;
typedef long long ll;
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) for (int i = (0), __tzg_##i = (b); i < __tzg_##i; ++i)
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define repd(i, a, b) for (int i = (a), __tzg_##i = (b); i <= __tzg_##i; ++i)
#define mst(a, b) memset(a, b, sizeof(a))
typedef pair<int, int> pii;
#define px first
#define py second
const ll mod = 1000000007;
const int MAXN = 220;
const int MAXM = 15;
const double eps = 1e-6;
#define mp(a, b) make_pair(a, b)
typedef vector<int> VI;
typedef vector<ll> VL;
typedef vector<pii> VPII;
typedef vector<string> VS;
typedef vector<VI> VVI;
typedef vector<VL> VVL;
struct LineMST {
  int count(int N, int L) {
    VVL pw(L + 1, VL(N * N + 1, 1L));
    rep(i, 2, pw.size()) rep(j, 1, pw[i].size()) {
      pw[i][j] = pw[i][j - 1] * i % mod;
    }
    VVL dp(N + 1, VL(L + 1, 0));
    dp[1] = VL(L + 1, 1);
    repd(i, 2, N) {
      repd(j, 1, L) {
        dp[i][j] = dp[i][j - 1];
        rep(k, 1, i) {
          dp[i][j] = ((dp[i][j]) + (dp[k][j - 1] * dp[i - k][j] % mod *
                                    pw[L - j + 1][k * (i - k) - 1] % mod)) %
                     mod;
        }
      }
    }
    ll res = dp[N][L];
    repd(i, 3, N) res = res * i % mod;
    return res;
  }
};
