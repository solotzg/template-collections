#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
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
#define px first
#define py second
#define __0(x) (!(x))
#define __1(x) (x)
typedef pair<int, int> pii;
const ll mod = 1000000007;
const int MAXM = 15;
const double eps = 1e-6;
const int inf = 0x3f3f3f3f;
#define mp(a, b) make_pair(a, b)
typedef vector<int> VI;
typedef vector<ll> VL;
typedef vector<pii> VPII;
typedef vector<string> VS;
typedef vector<VI> VVI;
typedef vector<VL> VVL;
const int N = 55;
VI eg[N];
int dis[N][N], dib[N], r[N][N];
int n;
struct Treestrat {
  int dfs(int u, int d) {
    if (r[u][d] != -1)
      return r[u][d];
    if (dib[u] <= d)
      return r[u][d] = 0;
    int res = dib[u] - d;
    rp(i, eg[u].size()) { res = max(res, 1 + dfs(eg[u][i], d + 1)); }
    return r[u][d] = res;
  }
  int roundcnt(VI tree, VI A, VI B) {
    n = tree.size() + 1;
    mst(dis, inf);
    rep(i, 1, n) {
      eg[i].push_back(tree[i - 1]);
      eg[tree[i - 1]].push_back(i);
      dis[i][tree[i - 1]] = 1;
      dis[tree[i - 1]][i] = 1;
    }
    rp(i, n) dis[i][i] = 0;
    rp(k, n) rp(i, n) rp(j, n) dis[i][j] =
        min(dis[i][j], dis[i][k] + dis[k][j]);
    rp(i, n) {
      dib[i] = inf;
      rp(j, B.size()) { dib[i] = min(dib[i], dis[i][B[j]]); }
    }
    int res = inf;
    rp(i, A.size()) {
      mst(r, -1);
      res = min(res, dfs(A[i], 0));
    }
    return res;
  }
};
