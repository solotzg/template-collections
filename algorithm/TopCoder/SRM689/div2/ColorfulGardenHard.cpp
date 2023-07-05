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
struct ColorfulGardenHard {
  const static int N = (1 << 15) + 10;
  int dp[16][16][N];
  int count(string g, string f) {
    int mmp[300];
    mst(mmp, 0);
    int n = g.size();
    rp(i, n)++ mmp[g[i]];
    VPII v;
    rp(i, 300) if (mmp[i]) v.pb(mp(i, mmp[i]));
    VI sum(v.size());
    int tol = 1;
    rp(i, v.size()) tol *= v[i].se + 1;
    sum[v.size() - 1] = 1;
    urp(i, v.size() - 2, 0) sum[i] = sum[i + 1] * (v[i + 1].se + 1);
    mst(dp, 0);
    dp[0][15][0] = 1;
    repd(cnt, 0, n - 1) rp(last, 16) rp(mask, tol) if (dp[cnt][last][mask]) {
      VI num(v.size());
      int p = mask;
      urp(i, v.size() - 1, 0) num[i] = p % (v[i].se + 1), p /= (v[i].se + 1);
      rp(i, num.size()) if (i != last && num[i] < v[i].se &&
                            v[i].fi != (int)f[cnt]) {
        dp[cnt + 1][i][mask + sum[i]] =
            add(dp[cnt + 1][i][mask + sum[i]], dp[cnt][last][mask]);
      }
    }
    int res = 0;
    rp(i, 16) res = add(res, dp[n][i][tol - 1]);
    return res;
  }
};

int main() {
#ifdef _TZG_DEBUG
  freopen("in.txt", "r", stdin);
#endif // __TZG_DEBUG
  ColorfulGardenHard *g = new ColorfulGardenHard;
  cout << g->count(

              "abcdefghijklmno", "zzzzzzzzzzzzzzz")
       << endl;
  return 0;
}
