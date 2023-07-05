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

const int N = 55, inf = 0x3f3f3f3f;
int mmp[N][N], vis[N][N];
struct MultiplicationTable2 {
  int minimalGoodSet(vector<int> table) {
    int n = (int)sqrt((double)table.size());
    rp(i, n) rp(j, n) { mmp[i][j] = table[i * n + j]; }
    int res = n;
    rp(a, n) {
      set<int> s;
      mst(vis, 0);
      s.insert(a);
      while (1) {
        VI ap;
        for (auto o : s) {
          for (auto p : s) {
            if (!vis[o][p])
              ap.pb(mmp[o][p]);
          }
        }
        for (auto o : s) {
          for (auto p : s) {
            vis[o][p] = 1;
          }
        }
        if (ap.size() == 0)
          break;
        s.insert(vtr(ap));
      }
      res = min(res, (int)s.size());
    }
    return res;
  }
};
