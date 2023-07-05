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
struct WalkingToSchool {
  vector<list<int>> g, rg;
  vector<int> from_0, from_1, to_1;
  const static int N = 2005;
  list<int> get_primes(int n) {
    static int vis[N];
    mst(vis, 0);
    list<int> r;
    repd(i, 2, n) {
      if (!vis[i])
        r.pb(i);
      for (int p = i + i; p <= n; p += i) {
        vis[p] = 1;
      }
    }
    return r;
  }
  void dfs(vector<list<int>> &mmp, vector<int> &vis, int x) {
    vis[x] = 1;
    for (int v : mmp[x])
      if (!vis[v])
        dfs(mmp, vis, v);
  }
  vector<int> color;
  int color_dfs(int x, int p, int c) {
    if (!to_1[x])
      return 1;
    if (color[x] == -1) {
      color[x] = c;
      for (int v : g[x]) {
        if (!color_dfs(v, p, (1 + c) % p))
          return 0;
      }
    } else {
      if (color[x] != c)
        return 0;
    }
    return 1;
  }
  int calc(int u, int p) {
    color.assign(g.size(), -1);
    return color_dfs(u, p, 0);
  }
  string canWalkExactly(int N, vector<int> from, vector<int> to) {
    const string ch = "Chores", fr = "Freedom";
    g.resize(N);
    rg.resize(N);
    rp(i, from.size()) {
      g[from[i]].pb(to[i]);
      rg[to[i]].pb(from[i]);
    }
    from_0.assign(N, 0);
    from_1.assign(N, 0);
    to_1.assign(N, 0);
    dfs(g, from_0, 0);
    dfs(g, from_1, 1);
    dfs(rg, to_1, 1);
    if (!from_0[1] || !from_1[0])
      return ch;
    for (int p : get_primes(N)) {
      if (calc(0, p))
        return ch;
    }
    return fr;
  }
};
int main() {
  WalkingToSchool *w = new WalkingToSchool;
  assert(w->canWalkExactly(2, {0, 1}, {1, 0}) == "Chores");
  return 0;
}
