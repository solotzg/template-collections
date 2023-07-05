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

struct SAP {
  const static int N = 1005;
  const static int M = 1005;
  const static int INF = 0x3f3f3f3f;
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

struct Singing {
  int solve(int N, int low, int high, vector<int> p) {
    _sap.init();
    int n = p.size();
    int st = 0, ed = N + 1;
    repd(i, 1, N) {
      if (i < low)
        _sap.add(st, i, SAP::INF);
      if (i > high)
        _sap.add(i, ed, SAP::INF);
    }
    rep(i, 1, n) if (p[i - 1] != p[i]) _sap.add(p[i - 1], p[i], 1),
        _sap.add(p[i], p[i - 1], 1);
    return _sap.sap(st, ed, N + 2);
  }
};
