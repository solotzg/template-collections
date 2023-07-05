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

const int N = 55, INF = 0x3f3f3f3f;
VI e[N];
int n;
int dp[N][N], cnt[N], ap[N][N];
struct TheKingsTree {
  int getNumber(VI p) {
    n = p.size() + 1;
    rp(i, p.size()) { e[p[i]].pb(i + 1); }
    urp(i, n - 1, 0) {
      cnt[i] = 1;
      for (int v : e[i])
        cnt[i] += cnt[v];
    }
    urp(i, n - 1, 0) {
      mst(dp[i], INF);
      mst(ap, INF);
      int s = cnt[i];
      int all = 0;
      ap[0][0] = 0;
      rp(j, e[i].size()) {
        int v = e[i][j];
        repd(c, 0, all) {
          repd(m, 0, cnt[v]) ap[j + 1][c + m] =
              min(ap[j + 1][c + m], ap[j][c] + dp[v][m]);
        }
        all += cnt[v];
      }
      repd(sum_of_red, 0, s - 1) {
        dp[i][sum_of_red + 1] =
            min(dp[i][sum_of_red + 1],
                ap[e[i].size()][sum_of_red] + sum_of_red + 1);
        dp[i][sum_of_red] = min(dp[i][sum_of_red],
                                ap[e[i].size()][sum_of_red] + s - sum_of_red);
      }
    }
    int res = INF;
    repd(i, 0, cnt[0]) res = min(res, dp[0][i]);
    return res;
  }
};

int main() { return 0; }
