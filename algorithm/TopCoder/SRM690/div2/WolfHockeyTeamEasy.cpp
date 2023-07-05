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

const static int MOD = 1e9 + 7;
inline int mul(int a, int b) { return (ll)a * b % MOD; }
inline int add(int a, int b) {
  return (a += b) >= MOD ? a - MOD : (a < 0 ? a + MOD : a);
}
inline void sadd(int &a, int b) { a = add(a, b); }
const int MAXN = 2010;
int A[MAXN];
void init() {
  A[0] = 1;
  rep(i, 1, MAXN) A[i] = mul(A[i - 1], i);
}
int dp[MAXN][MAXN][2];
struct WolfHockeyTeamEasy {
  int count(int N, int K) {
    init();
    mst(dp, 0);
    dp[1][0][0] = 1;
    int val;
    repd(i, 1, N) rep(j, 0, 2 * N - i) rp(k, 2) if ((val = dp[i][j][k])) {
      int nxt = 2 * N - (i + j) - 1;
      if (i < N) {
        sadd(dp[i + 1][j][k], val);
        if (!k && nxt >= K)
          sadd(dp[i + 1][j][1], val);
      }
      if (i > j) {
        sadd(dp[i][j + 1][k || (nxt >= K)], val);
      }
    }
    int res = dp[N][N][1];
    res = mul(res, 2);
    res = mul(res, A[N]);
    return res;
  }
};
