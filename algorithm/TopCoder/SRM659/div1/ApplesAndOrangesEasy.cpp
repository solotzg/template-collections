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
struct ApplesAndOrangesEasy {
  int maximumApples(int n, int k, vector<int> info) {
    VI ap(n, 0), cnt(n, k / 2);
    for (int a : info) {
      ap[a - 1] = 1;
    }
    rp(i, n) {
      rep(j, i, min(n, i + k)) if (ap[j])-- cnt[i];
      if (ap[i])
        continue;
      int o = 1;
      repd(j, max(0, i - k + 1), i) {
        if (!cnt[j]) {
          o = 0;
          break;
        }
      }
      if (o) {
        ap[i] = 1;
        repd(j, max(0, i - k + 1), i) { --cnt[j]; }
      }
    }
    return accumulate(vtr(ap), 0);
  }
};