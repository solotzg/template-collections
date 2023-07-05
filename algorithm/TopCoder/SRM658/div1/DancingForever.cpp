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

struct DancingForever {
  const static int N = 105;
  int n;
  int da[N][N], del[N], op[N];

  int belong[N], vis[N];
  int dfs(int u) {
    rp(v, n) if (da[u][v] && !vis[v]) {
      vis[v] = 1;
      if (belong[v] == -1 || dfs(belong[v])) {
        belong[v] = u;
        return 1;
      }
    }
    return 0;
  }
  void hungarian() {
    mst(belong, -1);
    rp(u, n) if (!del[u]) {
      mst(vis, 0);
      dfs(u);
    }
  }
  vector<int> getStablePairs(string x) {
    n = sqrt(x.size());
    mst(da, 0);
    rp(i, n) rp(j, n) if (x[i * n + j] == 'Y') da[i][j] = 1;
    mst(del, 0);
    while (1) {
      int f = 0;
      rp(i, n) if (!del[i]) {
        f = 1;
        break;
      }
      if (!f)
        return {-1};
      hungarian();
      mst(op, 0);
      rp(i, n) if (belong[i] != -1) op[belong[i]] = 1;
      f = 1;
      rp(u, n) if (op[u]) rp(v, n) if (da[u][v] && belong[v] == -1) {
        del[u] = 1;
        f = 0;
        break;
      }
      if (f) {
        VI res;
        rp(i, n) if (belong[i] != -1) res.pb(belong[i]), res.pb(i);
        return res;
      }
    }
    return {-1};
  }
};
