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

struct MinCostMaxFlow {
  const int static M = 155, N = M * 3;
  const int inf = 0x3f3f3f3f;
  struct Edge {
    int to, nxt;
    int cost, val;
    Edge(int _to = -1, int _nxt = -1, int _cost = 0, int _val = 0)
        : to(_to), nxt(_nxt), cost(_cost), val(_val) {}
  };
  Edge edge[(M * 3 + M * M * 2) * 2];
  int head[N], ecnt;
  int pre[N], pos[N], dis[N];
  void init() {
    memset(head, -1, sizeof head);
    ecnt = 0;
  }
  void add(int u, int v, int cost, int val) {
    edge[ecnt] = Edge(v, head[u], cost, val);
    head[u] = ecnt++;
    edge[ecnt] = Edge(u, head[v], -cost, 0);
    head[v] = ecnt++;
  }
  int minCost_maxFlow(int s, int t) {
    int flow = 0;
    int res = 0;
    while (spfa(s, t)) {
      int mn = inf;
      for (int i = t; i != s; i = pre[i]) {
        if (mn > edge[pos[i]].val)
          mn = edge[pos[i]].val;
      }
      for (int i = t; i != s; i = pre[i]) {
        edge[pos[i]].val -= mn;
        edge[pos[i] ^ 1].val += mn;
      }
      flow += mn;
      res += dis[t] * mn;
    }
    return res;
  }
  int spfa(int s, int t) {
    static int q[N * N], vis[N];
    int bg = 0, ed = 0;
    memset(vis, 0, sizeof vis);
    memset(pre, -1, sizeof pre);
    memset(dis, 0x3f, sizeof dis);
    dis[s] = 0;
    q[ed++] = s;
    while (bg < ed) {
      int u = q[bg++];
      vis[u] = 0;
      for (int i = head[u]; ~i; i = edge[i].nxt) {
        int v = edge[i].to;
        if (edge[i].val > 0 && dis[v] > dis[u] + edge[i].cost) {
          dis[v] = dis[u] + edge[i].cost;
          pre[v] = u;
          pos[v] = i;
          if (!vis[v]) {
            vis[v] = 1;
            q[ed++] = v;
          }
        }
      }
    }
    return pre[t] != -1;
  }
} cal;

struct TaroCutting {
  int getNumber(vector<int> height, vector<int> add, vector<int> device,
                int time) {
    cal.init();
    int n = device.size();
    int m = height.size();
    int cnt = 0;
    int S = cnt++, T = cnt++, NO = cnt++;
    VI tree(m), dv(n), day(time);
    rp(i, tree.size()) tree[i] = cnt++;
    rp(i, dv.size()) dv[i] = cnt++;
    rp(i, day.size()) day[i] = cnt++;
    cal.add(NO, T, 0, m);
    rp(i, m) {
      cal.add(S, tree[i], 0, 1);
      cal.add(tree[i], NO, time * add[i] + height[i], 1);
      rp(j, time) { cal.add(tree[i], day[j], (time - (j + 1)) * add[i], 1); }
    }
    rp(i, time) { rp(j, n) cal.add(day[i], dv[j], device[j], 1); }
    rp(i, n) cal.add(dv[i], T, 0, m);
    int res = cal.minCost_maxFlow(S, T);
    return res;
  }
};
