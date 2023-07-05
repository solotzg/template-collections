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
const int N = 55;
int e[N][N];
int sn[N], f = -1, n;
int q[N], top;
void dfs(int u, int pre) {
  if (f != -1)
    return;
  if (sn[u] != -1) {
    f = sn[u];
    return;
  }
  sn[u] = top;
  q[top++] = u;
  rp(i, n) if (e[u][i] && i != pre) {
    dfs(i, u);
    if (f != -1)
      return;
  }
  sn[u] = -1;
  --top;
}
struct SuccessfulMerger {
  int minimumMergers(vector<int> r) {
    n = r.size();
    mst(e, 0);
    rp(i, n) e[i][r[i]] = e[r[i]][i] = 1;
    mst(sn, -1);
    top = 0;
    dfs(0, -1);
    int snt = 0;
    rp(i, n) {
      int s = 0;
      rp(j, n) s += e[i][j];
      if (s == 1)
        ++snt;
    }
    if (f == -1) {
      if (n < 4)
        return 0;
      return n - snt - 1;
    } else {
      int cnt = 0;
      for (int i = f; i < top; ++i) {
        int s = 0;
        int ii = q[i];
        rp(j, n) s += e[ii][j];
        if (s > 2)
          ++cnt;
      }
      int o = (top - f);
      if (cnt != o) {
        return (n - snt - (top - f - 1) - 1) + (top - f - 2);
      } else {
        return n - snt - 1;
      }
    }
    return 0;
  }
};
