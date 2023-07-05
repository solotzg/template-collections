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

struct ColorfulGarden {
  vector<string> rearrange(vector<string> g) {
    int mmp[300];
    mst(mmp, 0);
    int n = g[0].size();
    rp(i, 2) rp(j, n)++ mmp[g[i][j]];
    vector<string> res(2, string(n, ' '));
    int pos = 0, s = 0;
    vector<PII> v;
    rp(i, 300) if (mmp[i]) { v.pb(mp(mmp[i], i)); }
    sort(vtr(v));
    urp(i, v.size() - 1, 0) rp(j, v[i].fi) {
      if (pos == n)
        pos = 0, s = 1;
      char c = v[i].se;
      res[s][pos] = c;
      pos++;
      s = 1 - s;
    }
    rp(i, 2) rp(j, n) {
      if (res[i][j] == res[1 - i][j])
        return {};
      if (j)
        if (res[i][j - 1] == res[i][j])
          return {};
      if (j < n - 1)
        if (res[i][j + 1] == res[i][j])
          return {};
    }
    return res;
  }
};

int main() {
#ifdef _TZG_DEBUG
  freopen("in.txt", "r", stdin);
#endif // __TZG_DEBUG
  ColorfulGarden g;
  auto s = g.rearrange({"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
                        "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"}

  );
  if (s.size())
    cout << s[0] << endl, cout << s[1] << endl;
  return 0;
}
