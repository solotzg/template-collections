#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;
struct SAP {
  const static int N = 210;
  const static int M = 210;
  const int INF = 0x3f3f3f3f;
  struct Edge {
    int v, w, nxt;
    Edge(int _v, int _w, int _nxt) : v(_v), w(_w), nxt(_nxt) {}
    Edge() {}
  } ee[(N * 2 + M * 2) * 2];
  int head[N], cnt;
  int cur[N], dis[N], gap[N], pre[N];
  void add(int u, int v, int w) {
    ee[cnt] = Edge(v, w, head[u]);
    head[u] = cnt++;
    ee[cnt] = Edge(u, 0, head[v]);
    head[v] = cnt++;
  }
  void init() {
    cnt = 0;
    memset(head, -1, sizeof head);
  }
  int sap(int start, int end, int nodenum) {
    memset(dis, 0, sizeof dis);
    memset(gap, 0, sizeof gap);
    memcpy(cur, head, sizeof cur);
    int u = pre[start] = start;
    int maxflow = 0, aug = -1;
    gap[0] = nodenum;
    while (dis[start] < nodenum) {
    loop:
      for (int &i = cur[u]; i != -1; i = ee[i].nxt) {
        int v = ee[i].v;
        if (ee[i].w && dis[u] == dis[v] + 1) {
          if (aug == -1 || aug > ee[i].w)
            aug = ee[i].w;
          pre[v] = u;
          u = v;
          if (v == end) {
            maxflow += aug;
            for (u = pre[u]; v != start; v = u, u = pre[u]) {
              ee[cur[u]].w -= aug;
              ee[cur[u] ^ 1].w += aug;
            }
            aug = -1;
          }
          goto loop;
        }
      }
      int mindis = nodenum;
      for (int i = head[u]; i != -1; i = ee[i].nxt) {
        int v = ee[i].v;
        if (ee[i].w && mindis > dis[v]) {
          cur[u] = i;
          mindis = dis[v];
        }
      }
      if ((--gap[dis[u]]) == 0)
        break;
      gap[dis[u] = mindis + 1]++;
      u = pre[u];
    }
    return maxflow;
  }
} _sap;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
int n, m;
int a[105], b[105];
int solve() {
  cin >> n >> m;
  int st = 0, ed = 209;
  int sa = 0, sb = 0;
  repd(i, 1, n) cin >> a[i], sa += a[i];
  repd(i, 1, n) cin >> b[i], sb += b[i];
  _sap.init();
  repd(i, 1, n) {
    _sap.add(st, i, a[i]);
    _sap.add(i + 105, ed, b[i]);
    _sap.add(i, 105 + i, 0x3f3f3f3f);
  }
  rp(i, m) {
    int u, v;
    cin >> u >> v;
    _sap.add(u, 105 + v, 0x3f3f3f3f);
    _sap.add(v, 105 + u, 0x3f3f3f3f);
  }
  if (sa != sb)
    return 0;
  int res = _sap.sap(st, ed, n * 2 + 2);
  return res == sa;
}
int main() {
  if (solve())
    puts("YES");
  else
    puts("NO");
  return 0;
}
