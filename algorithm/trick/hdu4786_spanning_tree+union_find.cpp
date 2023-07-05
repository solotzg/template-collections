/*
 * http://acm.hdu.edu.cn/showproblem.php?pid=4786
 * */
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>
using namespace std;
#define rep(i, a, b) for (int i(a), __tzg##i(b); i < __tzg##i; ++i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, 1 + b)
#define urp(i, a, b) for (int i(a), __tzg##i(b); i >= __tzg##i; --i)
typedef long long ll;
const static int N = 100005;
int dx;
int mmp[500] = {0, 1, 1};
int n, m;
int mp[N];
struct Edge {
  int u, v;
  int w;
  Edge(int _u = -1, int _v = -1, int _w = -1) : u(_u), v(_v), w(_w) {}
};
Edge edge[N];
void init() { repd(i, 1, n) mp[i] = i; }
int find(int x) { return mp[x] = mp[x] == x ? x : find(mp[x]); }
bool cmp1(const Edge &a, const Edge &b) { return a.w < b.w; }
bool cmp2(const Edge &a, const Edge &b) { return a.w > b.w; }
int solve() {
  int cl = 0, cr = 0;
  init();
  sort(edge, edge + m, cmp1);
  rp(i, m) {
    int u = find(edge[i].u);
    int v = find(edge[i].v);
    int w = edge[i].w;
    if (u != v) {
      mp[u] = v;
      cl += w;
    }
  }
  init();
  sort(edge, edge + m, cmp2);
  rp(i, m) {
    int u = find(edge[i].u);
    int v = find(edge[i].v);
    int w = edge[i].w;
    if (u != v) {
      mp[u] = v;
      cr += w;
    }
  }
  repd(i, 1, n) {
    if (find(i) != find(1))
      return 0;
  }
  rep(i, 2, dx) {
    if (mmp[i] >= cl && mmp[i] <= cr)
      return 1;
  }
  return 0;
}
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif
  for (dx = 3; (mmp[dx] = mmp[dx - 1] + mmp[dx - 2]) <= 100000; ++dx)
    ;
  int t, u, v, w;
  scanf("%d", &t);
  repd(_, 1, t) {
    printf("Case #%d: ", _);
    scanf("%d%d", &n, &m);
    rp(i, m) {
      scanf("%d%d%d", &u, &v, &w);
      edge[i] = Edge(u, v, w);
    }
    if (solve())
      puts("Yes");
    else
      puts("No");
  }
  return 0;
}
