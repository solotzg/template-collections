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

struct WolfCardGame {
  vector<int> createAnswer(int n, int k) {
    VI res;
    if (n % 2) {
      repd(i, 1, k) res.pb(2 * i);
    } else if (n % 3) {
      repd(i, 1, k) res.pb(3 * i);
    } else if (n % 5) {
      repd(i, 1, k) res.pb(5 * i);
    } else {
      // 30 60 90
      if (k <= 14) {
        repd(i, 1, k) res.pb(7 * i);
      } else {
        repd(i, 1, 14) res.pb(7 * i);
        if (n < 90)
          res.pb(1);
        else
          res.pb(2);
      }
    }
    return res;
  }
};
