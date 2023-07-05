#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
const int MAXN = 10010;
typedef long long LL;
#pragma comment(linker, "/STACK:1024000000,1024000000")
map<pair<int, int>, int> mmp;
struct _node {
  int qi, col;
} node[MAXN];
int fa[MAXN], n, cnt, sum[2], cd[MAXN];
const int dir[4][2] = {0, 1, 0, -1, -1, 0, 1, 0};
int getfa(int x) {
  if (fa[x] == x)
    return x;
  return fa[x] = getfa(fa[x]);
}
void init() {
  mmp.clear();
  cnt = 0;
  for (int i = 0; i <= n; ++i)
    fa[i] = i;
  sum[0] = sum[1] = 0;
  memset(cd, -1, sizeof cd);
}
void mdele(int col, int x, int y) {
  cd[mmp[make_pair(x, y)]] = -1;
  mmp[make_pair(x, y)] = 0;
  for (int i = 0; i < 4; ++i) {
    int xx = x + dir[i][0], yy = y + dir[i][1], v;
    if (v = mmp[make_pair(xx, yy)]) {
      if (node[v].col == col)
        mdele(col, xx, yy);
      else {
        v = getfa(v);
        node[v].qi++;
      }
    }
  }
}
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif
  int T;
  scanf("%d", &T);
  while (T--) {
    scanf("%d", &n);
    init();
    for (int i = 0; i < n; ++i) {
      int x, y, v, fu, fv, neg = 0, o;
      scanf("%d%d", &x, &y);
      mmp[make_pair(x, y)] = ++cnt;
      cd[cnt] = i & 1;
      node[cnt].col = i & 1;
      node[cnt].qi = 0;
      // printf("x=%d y=%d cnt=%d col=%d", x, y, cnt, node[cnt].col);
      for (int j = 0; j < 4; ++j) {
        int xx = x + dir[j][0], yy = y + dir[j][1];
        if (xx == 0 || yy == 0) {
          continue;
        } else if (v = mmp[make_pair(xx, yy)]) {
          if (node[v].col == node[cnt].col) {
            fu = getfa(cnt);
            fv = getfa(v);
            if (fv != fu) {
              neg += node[fv].qi - 1;
            } else
              neg--;
            fa[fv] = fu;
          } else {
            int fb = getfa(v);
            node[fb].qi--;
            if (node[fb].qi == 0)
              mdele(node[v].col, xx, yy);
          }
        } else {
          neg++;
        }
      }
      node[cnt].qi += neg;
      // printf(" qi=%d\n", node[cnt].qi);
      if (!node[cnt].qi)
        mdele(node[cnt].col, x, y);
    }
    for (int i = 1; i <= cnt; ++i)
      sum[0] += cd[i] == 0, sum[1] += cd[i] == 1;
    printf("%d %d\n", sum[0], sum[1]);
  }
  return 0;
}
