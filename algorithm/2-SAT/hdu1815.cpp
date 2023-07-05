#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>
using namespace std;
typedef long long ll;
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) for (int i = (0), __tzg_##i = (b); i < __tzg_##i; ++i)
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define repd(i, a, b) for (int i = (a), __tzg_##i = (b); i <= __tzg_##i; ++i)
#define mst(a, b) memset(a, b, sizeof(a))
typedef pair<int, int> pii;
#define px first
#define py second
const ll mod = 1000000007;
const double eps = 1e-6;
#define mp(a, b) make_pair(a, b)
typedef vector<int> VI;
#define px first
#define py second
const int MAXN = 505;
const int MAXM = 1005;
int N, A, B;
struct Pair {
  int x, y;
  void ipt(int k = 0) {
    scanf("%d%d", &x, &y);
    x += k;
    y += k;
  }
  Pair(int a = 0, int b = 0) : x(a), y(b) {}
  int dis(const Pair &a) const { return abs(x - a.x) + abs(y - a.y); }
} S1, S2, barns[MAXN], like[MAXM], hate[MAXM], len[MAXN];
vector<int> ee1[MAXN * 2], ee2[MAXN * 2];
int __dis, stk[MAXN * 2], top, bl[MAXN * 2], cbl, vis1[MAXN * 2],
    vis2[MAXN * 2], mx_dis;
void add(int u, int v) {
  ee1[u].push_back(v);
  ee2[v].push_back(u);
}
void init() {
  rp(i, N * 2) ee1[i].clear(), ee2[i].clear();
  mst(vis1, 0);
  mst(vis2, 0);
  top = 0;
  cbl = 0;
}
void setState(int dis) {
  rp(i, N) rep(j, i + 1, N) {
    if (len[i].x + len[j].x > dis)
      add(i << 1, j << 1 | 1), add(j << 1, i << 1 | 1);
    if (len[i].x + len[j].y + __dis > dis)
      add(i << 1, j << 1), add(j << 1 | 1, i << 1 | 1);
    if (len[i].y + len[j].x + __dis > dis)
      add(i << 1 | 1, j << 1 | 1), add(j << 1, i << 1);
    if (len[i].y + len[j].y > dis)
      add(i << 1 | 1, j << 1), add(j << 1 | 1, i << 1);
  }
}
void dfs1(int x) {
  vis1[x] = 1;
  rp(i, ee1[x].size()) if (!vis1[ee1[x][i]]) dfs1(ee1[x][i]);
  stk[top++] = x;
}
void dfs2(int x) {
  vis2[x] = 1;
  bl[x] = cbl;
  rp(i, ee2[x].size()) if (!vis2[ee2[x][i]]) dfs2(ee2[x][i]);
}
void prepare() {
  init();
  rp(i, A) {
    add(hate[i].x * 2, hate[i].y * 2 + 1);
    add(hate[i].x * 2 + 1, hate[i].y * 2);
    add(hate[i].y * 2, hate[i].x * 2 + 1);
    add(hate[i].y * 2 + 1, hate[i].x * 2);
  }
  rp(i, B) {
    add(like[i].x * 2, like[i].y * 2);
    add(like[i].x * 2 + 1, like[i].y * 2 + 1);
    add(like[i].y * 2, like[i].x * 2);
    add(like[i].y * 2 + 1, like[i].x * 2 + 1);
  }
}
int check() {
  rp(i, 2 * N) {
    if (!vis1[i])
      dfs1(i);
  }
  for (int i = top - 1; i >= 0; --i) {
    if (!vis2[stk[i]])
      dfs2(stk[i]), ++cbl;
  }
  rp(i, N) if (bl[i * 2] == bl[i * 2 + 1]) return 0;
  return 1;
}
void init_main() {
  mx_dis = -1;
  rp(i, N) {
    len[i].x = barns[i].dis(S1);
    len[i].y = barns[i].dis(S2);
    mx_dis = max(mx_dis, len[i].x);
    mx_dis = max(mx_dis, len[i].y);
  }
  __dis = S1.dis(S2);
}
int solve() {
  init_main();
  prepare();
  if (!check()) {
    return -1;
  }
  int st = 0, ed = mx_dis * 2 + __dis;
  while (st < ed) {
    int md = (st + ed) >> 1;
    prepare();
    setState(md);
    if (check())
      ed = md;
    else
      st = md + 1;
  }
  return st;
}
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif
  while (scanf("%d%d%d", &N, &A, &B) != EOF) {
    S1.ipt(), S2.ipt();
    rp(i, N) barns[i].ipt();
    rp(i, A) hate[i].ipt(-1);
    rp(i, B) like[i].ipt(-1);
    printf("%d\n", solve());
  }
  return 0;
}
