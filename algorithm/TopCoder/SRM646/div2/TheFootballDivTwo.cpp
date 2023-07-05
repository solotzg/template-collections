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

struct TheFootballDivTwo {

  int find(int yourScore, vector<int> scores, vector<int> numberOfTeams);
};

int TheFootballDivTwo::find(int ys, vector<int> s, vector<int> ns) {
  int n = 1 + accumulate(vtr(ns), 0);
  int le = 0, le3 = 0, le6 = 0, g6 = 0;
  rp(i, s.size()) {
    int sp = s[i], sns = ns[i];
    if (sp <= ys) {
      le += sns;
    } else if (sp <= ys + 3)
      le3 += sns;
    else if (sp <= ys + 6)
      le6 += sns;
    else
      g6 += sns;
  }
  n -= g6 * 2 + le * 2 + le3 + 2;
  n = n < 0 ? 0 : n;
  return g6 + 1 + (n + 1) / 2;
}
