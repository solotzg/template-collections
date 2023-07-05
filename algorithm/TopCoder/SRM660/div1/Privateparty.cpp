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
struct Privateparty {
  double getexp(vector<int> a) {
    static int vis[55];
    int n = a.size();
    double res = 0.0;
    rp(i, n) {
      mst(vis, 0);
      int t = 0, q = i;
      while (1) {
        vis[q] = 1;
        ++t;
        int p = a[q];
        if (vis[p]) {
          break;
        } else {
          q = p;
        }
      }
      double x = 1.0;
      rp(j, t) res += x, x /= -(j + 2);
    }
    return res;
  }
};
int main() { return 0; }
