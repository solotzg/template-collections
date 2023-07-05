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
struct CheeseRolling {
  int n;
  vector<string> wins;
  VI sub_mask[1 << 16];
  ll dp[16][1 << 16];
  ll go(int win, int mask) {
    ll &r = dp[win][mask];
    if (r != -1)
      return r;
    r = 0;
    rp(lose, n) if (lose != win && wins[win][lose] == 'Y') {
      for (int m : sub_mask[mask]) {
        if (_1((1 << win) & m) && _0((1 << lose) & m))
          r += 2 * go(win, m) * go(lose, mask - m);
      }
    }
    return r;
  }
  int cal(int x) {
    int r = 0;
    while (x) {
      ++r;
      x = (x - 1) & x;
    }
    return r;
  }
  void init() {
    rp(mask, 1 << n) {
      int a = cal(mask);
      sub_mask[mask].clear();
      for (int sub = mask & (mask - 1); sub > 0; sub = (sub - 1) & mask)
        if (cal(sub) * 2 == a)
          sub_mask[mask].pb(sub);
    }
  }
  vector<ll> waysToWin(vector<string> wins) {
    n = wins.size();
    this->wins = wins;
    init();
    mst(dp, -1);
    vector<ll> res;
    rp(i, n) dp[i][(1 << i)] = 1;
    rp(i, n) res.pb(go(i, (1 << n) - 1));
    return res;
  }
};
int main() {
  auto t = new CheeseRolling;
  assert(t->waysToWin({"NYNY", "NNYN", "YNNY", "NYNN"}) ==
         vector<ll>({8, 0, 16, 0}));
  assert(
      t->waysToWin({"NYNNNNYYNYYNNYNN", "NNNNNNNNNYYNYYNY", "YYNYYNNNNYYYYYYN",
                    "YYNNYYYNYNNYYYNY", "YYNNNYYNYYNNNNYY", "YYYNNNNYYNNYYNYN",
                    "NYYNNYNYNYNYYYYN", "NYYYYNNNYYNYNYYY", "YYYNNNYNNYYYYNNN",
                    "NNNYNYNNNNNNYYNY", "NNNYYYYYNYNYYYNN", "YYNNYNNNNYNNYNNY",
                    "YNNNYNNYNNNNNYNN", "NNNNYYNNYNNYNNYY", "YYNYNNNNYYYYYNNN",
                    "YNYNNYYNYNYNYNYN"}

                   ) ==
      vector<ll>({331616878592, 37267079168, 2426798866432, 2606831599616,
                  994941665280, 1162501849088, 1888166674432, 4632734203904,
                  832881524736, 84707409920, 3007127748608, 55490052096,
                  17818550272, 254672666624, 629921447936, 1959311671296}));
  return 0;
}
