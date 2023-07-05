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

struct ClassicProblem {
  const static int M = 31, X = 81 * 81 * 2 * 2;
  VPII s[M];
  ll dp[2][X];
  VI divide(int x) {
    VI r;
    rp(i, M) if ((1 << i) <= x) r.pb(i), x -= 1 << i;
    else break;
    rp(i, M) if (bit(x, i)) r.pb(i);
    return r;
  }
  long long maximalValue(vector<int> cnt, vector<int> w, vector<int> v,
                         int limit) {
    int n = cnt.size();
    rp(i, n) {
      VI r = divide(cnt[i]);
      for (int e : r)
        s[e].pb(mp(w[i], v[i]));
    }
    int cur = 0, pre = 1 - cur;
    mst(dp, 0);
    rp(pos, M) {
      cur = pre;
      pre = 1 - pre;
      mst(dp[cur], 0);
      int d = pos ? bit(limit, pos - 1) : 0;
      rp(i, X) dp[cur][i] = dp[pre][min(X - 1, 2 * i + d)];
      for (auto &e : s[pos]) {
        urp(i, X - 1, e.fi) dp[cur][i] =
            max(dp[cur][i], dp[cur][i - e.fi] + (1ll << pos) * (ll)e.se);
      }
    }
    return dp[cur][bit(limit, M - 1)];
  }
};

int main() {
  ClassicProblem *f = new ClassicProblem;
  assert(
      f->maximalValue(
          {17452733, 484,      607117,   44102867, 80917,    9955379,  154783,
           79,       757486,   1800008,  31397,    66532052, 5,        8351,
           30063,    538787,   66,       993,      28503205, 696813,   93438689,
           9474478,  14716355, 93438689, 53851,    75482618, 6856,     93438689,
           8033317,  31,       4,        2,        2957,     68,       536547,
           240613,   19,       290847,   57242428, 1722,     19160318, 692,
           73009,    5,        93438689, 1431,     8982},
          {12, 22, 6, 16, 14, 20, 10, 7,  5,  2,  12, 11, 11, 10, 9, 5,
           8,  3,  1, 18, 9,  7,  22, 21, 19, 12, 15, 20, 11, 17, 7, 14,
           11, 15, 5, 4,  22, 6,  11, 21, 17, 20, 22, 18, 12, 6,  6},
          {977866, 774579, 977866, 7,      54,     830,    3,      977866,
           977866, 27341,  977866, 290,    9,      445629, 977866, 6,
           525083, 1,      977866, 41533,  977866, 977866, 977866, 8,
           4203,   977866, 62884,  2,      9,      977866, 5005,   2,
           727,    9952,   977866, 977866, 275,    70,     61,     834,
           651,    977866, 977866, 977866, 977866, 16488,  977866},
          38504) == 37651752464ll);
  return 0;
}
