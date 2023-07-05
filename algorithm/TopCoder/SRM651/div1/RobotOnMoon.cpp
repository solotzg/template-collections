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

int dir[4][2] = {-1, 0, 1, 0, 0, -1, 0, 1};
int q[60 * 60][3], bg, ed;
int vis[60][60];
struct RobotOnMoon {
  int longestSafeCommand(vector<string> b) {
    int r = b.size(), c = b[0].size();
    auto check = [&](int a, int b) {
      return a >= 0 && a < r && b >= 0 && b < c;
    };
    int x = -1, y = -1;
    rp(i, r) rp(j, c) if (b[i][j] == 'S') x = i, y = j;
    rp(i, 4) {
      int xx = x, yy = y;
      while (1) {
        xx += dir[i][0];
        yy += dir[i][1];
        if (check(xx, yy)) {
          if (b[xx][yy] == '#')
            return -1;
        } else {
          break;
        }
      }
    }
    return r - 1 + c - 1;
  }
};
