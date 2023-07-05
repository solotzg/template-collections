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
const int N = 55;
int mmp[N][N];
int del[N];
int n;
void rm(int u) {
  del[u] = 1;
  rp(v, n) mmp[u][v] = mmp[v][u] = 0;
}
struct TreeCutting {
  string can(vector<int> par, vector<int> num) {
    if (cal(par, num))
      return "POSSIBLE";
    return "IMPOSSIBLE";
  }
  int cal(vector<int> &par, vector<int> &num) {
    mst(mmp, 0);
    n = num.size();
    rp(i, n - 1) mmp[i + 1][par[i]] = mmp[par[i]][i + 1] = 1;
    mst(del, 0);
    int co = 1;
    while (1) {
      co = 1;
      while (co) {
        co = 0;
        urp(e, n - 1, 0) if (!del[e]) {
          if (num[e] < 0) {
            int p = -1, f = 0;
            rp(v, n) if (mmp[e][v]) {
              p = v, ++f;
              if (f > 1)
                break;
            }
            if (f > 1)
              continue;
            if (f == 0)
              return 0;
            if (num[p] < 0) {
              num[p] += num[e];
            }
            if (num[p] > 0) {
              num[p] += num[e];
              if (num[p] < 1)
                return 0;
            }
            rm(e);
            co = 1;
            if (num[p] == 1)
              rm(p);
          }
        }
      }
      co = 1;
      while (co) {
        co = 0;
        urp(e, n - 1, 0) if (!del[e]) {
          if (num[e] > 0) {
            if (num[e] == 1) {
              rm(e);
              co = 1;
              continue;
            }
            int p = -1, f = 0;
            rp(v, n) if (mmp[e][v]) {
              p = v, ++f;
              if (f > 1)
                break;
            }
            if (f > 1)
              continue;
            if (f == 0)
              return 0;
            if (num[p] > 0)
              return 0;
            if (num[p] < 0) {
              num[p] += num[e];
              if (num[p] < 1)
                return 0;
            }
            rm(e);
            co = 1;
            if (num[p] == 1)
              rm(p);
          }
        }
      }
      int ck = 0;
      rp(i, n) if (!del[i]) {
        ck = 1;
        break;
      }
      if (ck == 0)
        return 1;
    }
  }
};

int main() {
  TreeCutting t;
  cout << t.can({0, 0, 0, 0, 1, 1,  2,  3,  3,  3, 4,
                 4, 4, 5, 9, 9, 14, 14, 14, 16, 20},
                {-1, 3, -1, -1, -1, -1, -1, 1,  1, -1, -1,
                 -1, 3, 1,  -1, 1,  8,  -1, -1, 4, -1, -1})
       << endl;
  return 0;
}
