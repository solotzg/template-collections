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

const int INF = 1.5e9;
struct Node {
  int v, x, y;
  bool operator<(const Node &a) const { return v > a.v; }
};
int dx[] = {0, 0, 1, -1};
int dy[] = {1, -1, 0, 0};
struct TheGridDivOne {
  pair<PII, int> mmp[150][150];
  int find(vector<int> x, vector<int> y, int k) {
    set<PII> blk;
    VI tx, ty;
    rp(i, x.size()) repd(j, -1, 1) tx.pb(x[i] + j), ty.pb(j + y[i]);
    rp(i, x.size()) blk.insert(mp(x[i], y[i]));
    tx.pb(0), ty.pb(0);
    tx.pb(INF);
    sort(vtr(tx)), sort(vtr(ty));
    int n = unique(vtr(tx)) - tx.begin();
    int m = unique(vtr(ty)) - ty.begin();
    rp(i, n) rp(j, m) mmp[i][j] =
        mp(mp(tx[i], ty[j]), blk.find(mp(tx[i], ty[j])) != blk.end());
    int ox = lower_bound(tx.begin(), tx.begin() + n, 0) - tx.begin(),
        oy = lower_bound(ty.begin(), ty.begin() + m, 0) - ty.begin();
    vector<VI> dis(n, VI(m, INF));
    dis[ox][oy] = 0;
    priority_queue<Node> q;
    q.push({0, ox, oy});
    int res = 0;
    while (!q.empty()) {
      auto nd = q.top();
      int px = nd.x, py = nd.y;
      auto &omp = mmp[px][py];
      int v = nd.v;
      q.pop();
      if (nd.v != dis[px][py])
        continue;
      res = max(res, omp.fi.fi);
      //			printf("cur : %d %d %d\n", omp.fi.fi, omp.fi.se,
      // v);
      int xx, yy;
      rp(i, 4) {
        xx = px + dx[i], yy = py + dy[i];
        if (_0(xx >= 0 && xx < n && yy >= 0 && yy < m))
          continue;
        auto &fmp = mmp[xx][yy];
        if (fmp.se == 1)
          continue;
        if (fmp.fi.fi == INF) {
          res = max(res, (k - v) + omp.fi.fi);
        } else {
          int diffx = abs(fmp.fi.fi - omp.fi.fi);
          int ad = diffx + abs(fmp.fi.se - omp.fi.se) + v;
          if (dis[xx][yy] > ad) {
            if (ad > k) {
              res = max(res,
                        omp.fi.fi + min(k - v, diffx) *
                                        (fmp.fi.fi - omp.fi.fi < 0 ? -1 : 1));
              continue;
            }
            dis[xx][yy] = ad;
            q.push({ad, xx, yy});
          }
        }
      }
    }
    return res;
  }
};

int main() {
  TheGridDivOne t;
  cout << t.find({-1000000000, -1000000000, 1000000000, 1000000000, 1, 1, 1, 1,
                  1, 1},
                 {-1000000000, 1000000000, -1000000000, 1000000000, -2, -1, 0,
                  1, 2, 3},
                 1000000000)
       << endl;
}
