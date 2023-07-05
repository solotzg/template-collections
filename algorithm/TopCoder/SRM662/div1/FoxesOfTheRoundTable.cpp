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
struct FoxesOfTheRoundTable {
  VI res;
  int cal(VPII &dh, int mx_diff) {
    res.clear();
    res.pb(dh[0].se);
    VI la, lb;
    int a = dh[0].fi, b = a, n = dh.size();
    rep(i, 1, n - 1) {
      int tp = dh[i].fi;
      if (tp - a <= mx_diff && tp - b <= mx_diff) {
        if (tp - a > tp - b) {
          la.pb(dh[i].se);
          a = tp;
        } else {
          lb.pb(dh[i].se);
          b = tp;
        }
      } else if (tp - a <= mx_diff) {
        la.pb(dh[i].se);
        a = tp;
      } else if (tp - b <= mx_diff) {
        lb.pb(dh[i].se);
        b = tp;
      } else {
        return 0;
      }
    }
    int p = dh[n - 1].fi;
    if (p - a > mx_diff || p - b > mx_diff)
      return 0;
    la.pb(dh[n - 1].se);
    res.insert(res.end(), vtr(la));
    urp(i, lb.size() - 1, 0) res.pb(lb[i]);
    return 1;
  }
  vector<int> minimalDifference(vector<int> h) {
    VPII dh;
    rp(i, h.size()) dh.pb(mp(h[i], i));
    sort(vtr(dh));
    repd(mx_diff, 1, 1000) {
      if (cal(dh, mx_diff))
        break;
    }
    return res;
  }
};
