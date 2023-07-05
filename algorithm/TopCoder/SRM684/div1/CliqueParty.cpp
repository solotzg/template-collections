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

struct CliqueParty {
  int sum[55];
  int maxsize(vector<int> a, int k) {
    int n = a.size();
    sort(vtr(a));
    int res = -1;
    rp(i, n) rep(j, i + 1, n) {
      int mx = a[j] - a[i];
      int mn = (mx + k - 1) / k;
      mst(sum, 0);
      int tp = 0;
      repd(p, i + 1, j) {
        rep(q, i, p) if (a[p] - a[q] >= mn) {
          sum[p] = max(sum[p], sum[q] + 1);
          tp = max(sum[p], tp);
        }
      }
      res = max(res, tp + 1);
    }
    return res;
  }
};

int main() {
  CliqueParty c;
  cout << c.maxsize({748, 272, 810, 279, 467, 472, 218, 853, 552, 749, 63, 179,
                     831, 335, 485, 785, 675, 416, 558, 577, 531, 691, 39, 610,
                     431, 738, 616, 293, 717, 380, 744, 626, 463, 784},
                    7)
       << endl;
  return 0;
}
