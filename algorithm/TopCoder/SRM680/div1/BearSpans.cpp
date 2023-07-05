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
struct BearSpans {
  VI res;
  const static int N = 1005;
  int mmp[N][N], cm;
  void add(int u, int v) {
    res.pb(u);
    res.pb(v);
    mmp[u][v] = mmp[v][u] = 1;
    ++cm;
  }
  VI findAnyGraph(int n, int m, int k) {
    int cnt = n;
    {
      int x = n, p = 0;
      while (x != 1)
        x /= 2, ++p;
      if (p < k)
        return {-1};
    }
    mst(mmp, 0);
    cm = 0;
    VI ss;
    rp(i, n) ss.pb(i);
    rp(i, k - 1) {
      VI p;
      for (int j = 0; j < (int)ss.size(); j += 2) {
        if (j + 1 < (int)ss.size())
          add(ss[j], ss[j + 1]), p.pb(ss[j]);
        else
          add(ss[0], ss[j]);
      }
      cnt /= 2;
      ss = move(p);
    }
    rep(i, 1, ss.size()) { add(0, ss[i]); }
    rp(i, n) rep(j, i + 1, n) if (!mmp[i][j]) {
      if (cm == m)
        goto goo;
      res.pb(i), res.pb(j);
      ++cm;
    }
  goo:;
    rp(i, res.size()) { ++res[i]; }
    return res;
  }
};
int main() {
  auto a = new BearSpans;
  VI v = move(a->findAnyGraph(9, 12, 3));
  cout << v.size() << endl << endl;
  for (int i : v) {
    cout << i << endl;
  }
  return 0;
}
