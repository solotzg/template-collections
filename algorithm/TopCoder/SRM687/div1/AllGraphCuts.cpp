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

struct UnionFind {
  vector<int> data;
  void init(int n) { data.assign(n, -1); }
  bool union_set(int x, int y) {
    x = root(x);
    y = root(y);
    if (x != y) {
      if (data[y] < data[x])
        swap(x, y);
      data[x] += data[y];
      data[y] = x;
    }
    return x != y;
  }
  bool find_set(int x, int y) { return root(x) == root(y); }
  int root(int x) { return data[x] < 0 ? x : data[x] = root(data[x]); }
  int size(int x) { return -data[root(x)]; }
};

struct AllGraphCuts {
  vector<int> findGraph(vector<int> x) {
    int n = (int)sqrt(x.size() * 1.0);
    int inf = 0x3f3f3f3f;
    vector<VI> mmp(n, VI(n));
    int f = 1;
    rp(i, n) rp(j, n) { mmp[i][j] = x[i * n + j]; }
    rp(i, n) rp(j, n) {
      if (mmp[i][j] != mmp[j][i])
        f = 0;
      if (mmp[i][i] != 0)
        f = 0;
    }
    rp(i, n) mmp[i][i] = inf;
    rp(a, n) rp(b, n) rp(c, n) if (mmp[a][c] < min(mmp[a][b], mmp[b][c])) {
      f = 0;
    }
    if (!f)
      return {-1};
    UnionFind u;
    u.init(n);
    vector<pair<int, PII>> v;
    rp(i, n) rep(j, i + 1, n) v.pb(mp(mmp[i][j], mp(i, j)));
    sort(vtr(v));
    reverse(vtr(v));
    VI res;
    for (auto &o : v) {
      int w = o.fi;
      int i = o.se.fi, j = o.se.se;
      //			printf("%d %d %d\n", w, i, j);
      if (u.union_set(i, j)) {
        res.pb(w * n * n + i * n + j);
      }
    }
    return res;
  }
};
