#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
typedef long long ll;
int mp[1000005];
struct BearDartsDiv2 {
  long long count(vector<int> w) {
    ll res = 0;
    int len = w.size();
    mst(mp, 0);
    rep(i, 2, len) {
      rep(j, i + 1, len) {
        if (w[j] % w[i] == 0)
          mp[w[j] / w[i]]++;
      }
    }
    if ((ll)w[0] * w[1] <= 1000000ll)
      res += mp[w[0] * w[1]];
    rep(i, 2, len - 2) {
      rep(k, i + 1, len) {
        if (w[k] % w[i] == 0)
          --mp[w[k] / w[i]];
      }
      rep(j, 0, i) {
        ll p = (ll)w[i] * w[j];
        if (p > 1000000ll)
          continue;
        res += mp[p];
      }
    }
    return res;
  }
};
