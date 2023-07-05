#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

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
const static int MOD = 1e9 + 7;

#define NONE 0
#define TRUE 1
#define FALSE 2
struct LCMGCD {
  VPII da;
  PII ta;
  array<int, 9> cnt;
  map<array<int, 9>, int> mmp;
  PII go(int x) {
    int n2 = 0, n3 = 0;
    while (x % 2 == 0)
      ++n2, x /= 2;
    while (x % 3 == 0)
      ++n3, x /= 3;
    return mp(n2, n3);
  }
  PII gao(int x, int y) {
    PII a = mp(x / 3, x % 3), b = mp(y / 3, y % 3);
    PII gcd = mp(min(a.fi, b.fi), min(a.se, b.se));
    PII lcm = mp(max(a.fi, b.fi), max(a.se, b.se));
    return mp(gcd.fi * 3 + gcd.se, lcm.fi * 3 + lcm.se);
  }
  int dfs(int n) {
    int &res = mmp[cnt];
    if (res != NONE)
      return res;
    if (n == 1)
      return res = cnt[4] == 1 ? TRUE : FALSE;
    rp(i, 9) if (cnt[i]) {
      --cnt[i];
      rep(j, i, 9) if (cnt[j]) {
        PII g = gao(i, j);
        {
          --cnt[j], ++cnt[g.fi];
          if (dfs(n - 1) == TRUE)
            return res = TRUE;
          ++cnt[j], --cnt[g.fi];
        }
        {
          --cnt[j], ++cnt[g.se];
          if (dfs(n - 1) == TRUE)
            return res = TRUE;
          ++cnt[j], --cnt[g.se];
        }
      }
      ++cnt[i];
    }
    return res = FALSE;
  };
  string isPossible(VI x, int t) {
    int n = x.size();
    rp(i, n) da.pb(go(x[i]));
    ta = go(t);
    if (solve())
      return "Possible";
    return "Impossible";
  }
  int cmp(int x) { return (x > 0) - (x < 0) + 1; }
  int solve() {
    fill(vtr(cnt), 0);
    for (auto &e : da) {
      int o2 = cmp(e.fi - ta.fi), o3 = cmp(e.se - ta.se);
      ++cnt[o2 * 3 + o3];
    }
    int sum = 0;
    rp(i, 9) {
      cnt[i] = min(cnt[i], 4);
      sum += cnt[i];
    }
    return dfs(sum) == TRUE;
  }
};

int main() {
#ifdef _TZG_DEBUG
  freopen("in.txt", "r", stdin);
  // freopen("out_b.txt", "w", stdout);
#endif // _TZG_DEBUG
  LCMGCD l;
  cout << l.isPossible({3,        944784,   256,    162,    32,    20736,
                        10368,    15116544, 162,    177147, 59049, 20736,
                        354294,   729,      1728,   512,    162,   20736,
                        15116544, 18,       9,      81,     2304,  629856,
                        118098,   81,       5184,   4374,   6,     243,
                        1458,     41472,    177147, 162,    5184,  90699264,
                        34992,    2592,     3456,   648,    81,    81,
                        256,      3779136,  81,     41472,  64,    7558272,
                        1728,     2},
                       324)
       << endl;
  return 0;
}
