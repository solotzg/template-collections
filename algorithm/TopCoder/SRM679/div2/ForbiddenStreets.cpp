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
struct ForbiddenStreets {
  vector<int> find(int, vector<int>, vector<int>, vector<int>);
};
const static int N = 105, INF = 0x3f3f3f3f;
vector<PII> e[N];
int ways[N][N], dis[N][N];
;
VI ForbiddenStreets::find(int n, VI A, VI B, VI time) {
  rp(i, A.size()) e[A[i]].pb(mp(B[i], time[i])), e[B[i]].pb(mp(A[i], time[i]));
  priority_queue<tuple<int, int>> q;
  mst(ways, 0);
  mst(dis, INF);
  rp(start, n) {
    q.push(make_tuple(0, start));
    ways[start][start] = 1;
    dis[start][start] = 0;
    while (q.empty() == false) {
      auto ele = q.top();
      q.pop();
      int ds, to;
      tie(ds, to) = ele;
      ds = -ds;
      if (ds > dis[start][to])
        continue;
      for (auto &v : e[to]) {
        if (dis[start][v.fi] == dis[start][to] + v.se) {
          ways[start][v.fi] += ways[start][to];
        } else if (dis[start][v.fi] > dis[start][to] + v.se) {
          dis[start][v.fi] = dis[start][to] + v.se;
          ways[start][v.fi] = ways[start][to];
          q.push(make_tuple(-dis[start][v.fi], v.fi));
        }
      }
    }
  }
  VI res;
  rp(i, A.size()) {
    int a = A[i], b = B[i], ans = 0;
    rp(u, n) rp(v, n) if (dis[u][v] == dis[u][a] + dis[b][v] + time[i]) {
      if (ways[u][v] == ways[u][a] * ways[b][v])
        ++ans;
    }
    res.pb(ans);
  }
  return res;
}
int main() {
  auto t = new ForbiddenStreets;
  assert(t->find(3, {0, 1, 2}, {1, 2, 0}, {1, 2, 3}) == VI({1, 1, 0}));
  return 0;
}
