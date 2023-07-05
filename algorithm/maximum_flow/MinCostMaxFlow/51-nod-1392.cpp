#include <algorithm>
#include <cstdio>
#include <cstring>
using namespace std;
struct MinCostMaxFlow {
  const int static N = 205 * 2;
  const int inf = 0x3f3f3f3f;
  struct Edge {
    int to, nxt;
    int cost, val;
    Edge(int _to = -1, int _nxt = -1, int _cost = 0, int _val = 0)
        : to(_to), nxt(_nxt), cost(_cost), val(_val) {}
  };
  Edge edge[N * N * 2];
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
};

#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
typedef long long ll;
const int N = 205, inf = 0x3f3f3f3f / 200;
int L[N], W[N], n;
MinCostMaxFlow fk;
int main() {
  freopen("in.txt", "r", stdin);
  scanf("%d", &n);
  int st = 2 * n, ed = st + 1;
  fk.init();
  rp(i, n) {
    scanf("%d%d", L + i, W + i);
    fk.add(st, i, 0, 1);
    fk.add(i, ed, 0, 1);
    fk.add(i + n, ed, L[i] * W[i], 1);
    fk.add(i, i + n, -inf, 1);
  }
  rp(i, n) rp(j, n) if (i != j) {
    if (L[i] <= L[j] && W[i] <= W[j]) {
      if (L[i] == L[j] && W[i] == W[j]) {
        if (i < j)
          fk.add(i + n, j, 0, 1);
      } else
        fk.add(i + n, j, 0, 1);
    }
  }
  int res = fk.minCost_maxFlow(st, ed) + inf * n;
  printf("%d\n", res);
  return 0;
}
