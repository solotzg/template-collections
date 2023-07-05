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
struct FleetFunding {
  int m;
  int cal(int w, VI k, VI &a, VI &b) {
    repd(i, 1, m) {
      int p = w;
      while (p > 0) {
        int o = -1, u;
        rp(j, a.size()) {
          if (k[j] > 0 && a[j] <= i && b[j] >= i) {
            if (o == -1 || b[j] < b[o])
              o = j;
          }
        }
        if (o == -1)
          return 0;
        u = min(k[o], p);
        k[o] -= u;
        p -= u;
      }
    }
    return 1;
  }
  int maxShips(int m, vector<int> k, vector<int> a, vector<int> b) {
    int dn = 0, up = 50 * 1e6;
    this->m = m;
    while (dn < up) {
      int mid = (dn + up + 1) / 2;
      if (cal(mid, k, a, b))
        dn = mid;
      else
        up = mid - 1;
    }
    return up;
  }
};
int main() {
  auto t = FleetFunding();
  cout << t.maxShips(
              84457, {374778, 169109, 529984, 498638, 29462,  465769, 29285,
                      394948, 307944, 459189, 349316, 766627, 686851, 404285,
                      850199, 359378, 3014,   248257, 558856, 27393,  32370,
                      574862, 337134, 965567, 32223,  935678, 389898, 973497,
                      990575, 483190, 122899, 857994, 991217, 149619, 415870},
              {92,   4997,  11,   28545, 933, 210,  2,   124,   114,
               4513, 32959, 1,    57,    17,  13,   133, 1843,  41851,
               3,    9296,  9757, 28,    3,   769,  10,  11102, 683,
               6173, 11821, 3982, 214,   2,   4304, 439, 1998},
              {81034, 54474, 70239, 79597, 47317, 82938, 83883, 74652, 84033,
               84422, 84456, 84457, 81095, 83743, 79210, 84255, 84455, 45596,
               84456, 82959, 67062, 80330, 44090, 84445, 84454, 84450, 45931,
               77190, 83025, 83567, 83639, 84333, 83355, 70982, 84456})
       << endl;
  return 0;
}
