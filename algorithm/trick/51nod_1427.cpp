// #pragma comment(linker, "/STACK:200000000,200000000")
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stack>
#include <vector>
using namespace std;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
typedef long long ll;
const static int N = 300005;
const static int M = 300005;
const int inf = 0x3f3f3f3f;
int head[N], ecnt;
struct Edge {
  int to, nxt;
  Edge(int _to = -1, int _nxt = -1) : to(_to), nxt(_nxt) {}
} edge[N * 2];
void add(int u, int v) { edge[ecnt] = Edge(v, head[u]), head[u] = ecnt++; }
int fa[N], cur_top;
int dep_pos[N], vis[N], mx_dep[N];
int dfs(int u, int pre) {
  int dep = 0;
  dep_pos[u] = u;
  fa[u] = cur_top;
  vis[u] = 1;
  for (int i = head[u]; i != -1; i = edge[i].nxt) {
    int v = edge[i].to;
    if (v == pre)
      continue;
    int tp = dfs(v, u) + 1;
    if (tp > dep) {
      dep = tp;
      dep_pos[u] = dep_pos[v];
    }
  }
  return dep;
}
int n, m, q;
void init() {
  memset(head, -1, sizeof head);
  ecnt = 0;
  mst(vis, 0);
  repd(i, 1, n) fa[i] = i;
}
int find(int x) { return fa[x] = fa[x] == x ? x : find(fa[x]); }
void solve() {
  scanf("%d%d%d", &n, &m, &q);
  init();
  rp(i, m) {
    int u, v;
    scanf("%d%d", &u, &v);
    add(u, v);
    add(v, u);
  }
  repd(i, 1, n) {
    if (!vis[i]) {
      cur_top = i;
      dfs(i, -1);
      mx_dep[cur_top] = dfs(dep_pos[i], -1);
    }
  }
  while (q--) {
    int c, a, b, x, y;
    scanf("%d", &c);
    if (c == 1) {
      scanf("%d", &a);
      printf("%d\n", mx_dep[find(a)]);
    } else {
      scanf("%d%d", &a, &b);
      x = find(a);
      y = find(b);
      if (x == y)
        continue;
      fa[y] = x;
      mx_dep[x] = max((mx_dep[x] + 1) / 2 + (mx_dep[y] + 1) / 2 + 1, mx_dep[x]);
      mx_dep[x] = max(mx_dep[x], mx_dep[y]);
    }
  }
}
int main() {
  // freopen("in.txt", "r", stdin);
  solve();
  return 0;
}
