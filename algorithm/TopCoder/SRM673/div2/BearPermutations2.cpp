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
struct BearPermutations2 {
  const static int MAXN = 105;
  int dp[MAXN];
  int a[MAXN], comb[MAXN][MAXN];
  int getSum(int N, int MOD) {
    a[0] = 1;
    rep(i, 1, MAXN) a[i] = (ll)a[i - 1] * i % MOD;
    rp(i, MAXN) {
      comb[i][0] = comb[i][i] = 1;
      rep(j, 1, i) comb[i][j] = (comb[i - 1][j - 1] + comb[i - 1][j]) % MOD;
    }
    dp[1] = dp[0] = 0;
    repd(n, 2, N) {
      int &d = dp[n];
      d = 0;
      repd(p, 0, n - 1) {
        int v = 0;
        int la = p, lb = n - 1 - p;
        rp(p1, la) rp(p2, lb) {
          int dis = (p2 + p + 1) - (p1);
          v = (v + (ll)dis * a[la - 1] % MOD * a[lb - 1] % MOD) % MOD;
        }
        v = (v + (ll)dp[la] * a[lb] % MOD) % MOD;
        v = (v + (ll)dp[lb] * a[la] % MOD) % MOD;
        v = (ll)v * comb[n - 1][la] % MOD;
        d = (d + v) % MOD;
      }
    }
    return dp[N];
  }
};
int main() {
  BearPermutations2 b;
  assert(b.getSum(4, 502739849) == 38);
  return 0;
}
