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
struct ANewHope {
  const static int N = 2505;
  int pos[N];
  int count(VI first, VI last, int d) {
    int n = first.size();
    rp(i, n) pos[last[i]] = i;
    rp(i, n) first[i] = pos[first[i]], last[i] = i;
    int ans = 1;
    VI cur = first, nxt = cur;
    set<int> s;
    while (cur != last) {
      s.clear();
      rp(i, n - d) { s.insert(cur[i]); }
      rp(i, n) {
        if (n - d + i < n)
          s.insert(cur[n - d + i]);
        nxt[i] = *s.begin();
        s.erase(s.begin());
      }
      swap(cur, nxt);
      ++ans;
    }
    return ans;
  }
};
