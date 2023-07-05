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

struct FriendlyRobot {
  const static int N = 305;
  int dp[N][N];
  string instructions;
  int calc(int from, int rest) {
    int &d = dp[from][rest];
    if (d != -1)
      return d;
    d = 0;
    int x = 0, y = 0;
    rep(next, from, instructions.size()) {
      switch (instructions[next]) {
      case 'U':
        ++y;
        break;
      case 'D':
        --y;
        break;
      case 'L':
        --x;
        break;
      case 'R':
        ++x;
        break;
      }
      int tp = abs(x) + abs(y);
      if (_1(tp & 1) || _1(tp / 2 > rest))
        continue;
      d = max(d, 1 + calc(next + 1, rest - tp / 2));
    }
    return d;
  }
  int findMaximumReturns(string _instructions, int changesAllowed) {
    mst(dp, -1);
    instructions = _instructions;
    return calc(0, changesAllowed);
  }
};
int main() { return 0; }
