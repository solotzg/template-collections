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

struct BuildingTowers {
  ll maxHeight(int n, int k, VI x, VI t);
};

ll BuildingTowers::maxHeight(int N, int K, VI x, VI _t) {
  vector<ll> t(_t.size());
  rp(i, t.size()) t[i] = (ll)_t[i];
  if (!x.size())
    return 1ll * (N - 1) * K;
  if (x.front() != 1)
    x.insert(x.begin(), 1), t.insert(t.begin(), 0ll);
  else
    t[0] = 0ll;
  if (x.back() != N)
    x.pb(N), t.pb(1ll << 60);
  int f = 1;
  int n = x.size();
  ll k = K;
  while (f) {
    f = 0;
    rep(i, 0, n - 1) {
      ll p0 = x[i], p1 = x[i + 1];
      ll h0 = t[i], h1 = t[i + 1];
      if (h0 + (p1 - p0) * k < h1) {
        t[i + 1] = h0 + (p1 - p0) * k;
        f = 1;
      }
    }
    rep(i, 0, n - 1) {
      ll p0 = x[i], p1 = x[i + 1];
      ll h0 = t[i], h1 = t[i + 1];
      if (h1 + (p1 - p0) * k < h0) {
        t[i] = h1 + (p1 - p0) * k;
        f = 1;
      }
    }
  }
  ll res = *max_element(vtr(t));
  rep(i, 0, n - 1) {
    ll p = x[i + 1] - x[i];
    if (!p)
      continue;
    ll h0 = t[i], h1 = t[i + 1];
    ll x = (p * k + h1 - h0) / (k * 2ll);
    ll y = k * x + h0;
    res = max(res, y);
    x = (p * k + h1 - h0 + k * 2ll - 1) / (k * 2ll);
    y = -k * x + k * p + h1;
    res = max(res, y);
  }
  return res;
}

int main() {
  BuildingTowers b;
  cout << b.maxHeight(
              140135, 7402,
              {890,    2214,   3253,   7565,   15069,  19374,  23499,  33300,
               34154,  38019,  40043,  42452,  53506,  57821,  57973,  59216,
               61952,  63649,  66883,  67242,  67605,  72512,  73317,  73906,
               74305,  76018,  82607,  83439,  98326,  107978, 117267, 119849,
               120884, 121745, 123046, 126534, 126920, 127197, 130167, 133777},
              {27988,   1300303,   9201,      7869802,  99975335,  3013,
               1967749, 9,         1915469,   84935,    1266939,   360054376,
               2189419, 174136496, 26880583,  81,       448,       167947427,
               7650,    73292915,  411420,    23730023, 2,         4991705,
               1,       14781,     67977326,  44821809, 240698879, 70082,
               366,     28347,     290515471, 36001,    27473,     3,
               21353,   50187,     15,        2490861})
       << endl;
  return 0;
}
