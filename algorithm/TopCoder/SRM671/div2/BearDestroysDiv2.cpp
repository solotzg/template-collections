#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
typedef long long ll;
struct BearDestroysDiv2 {
  int MOD, W, H;
  int dp[8][41][1 << 7];
  int pow2[8 * 41];
  int add(int a, int b) {
    a += b;
    return a >= MOD ? a - MOD : a;
  }
  int calc(int x, int y, int mask) {
    ll res = 0;
    if (x == W - 1 && y == H - 1)
      return 0;
    else if (x >= W) {
      return calc(0, y + 1, mask);
    } else {
      if (dp[x][y][mask] != -1)
        return dp[x][y][mask];
      int umask = (~(1 << x)) & mask;
      int smask = mask | (1 << x);
      int R = -1, D = -1, O = -1;
      int down = 1, right = 1;
      if ((1 << x) & mask || y == H - 1)
        down = 0;
      if ((1 << x) & mask || x == W - 1 || (1 << (x + 1)) & mask)
        right = 0;
      O = calc(x + 1, y, umask);
      if (right)
        R = add(pow2[W * H - y * W - x - 1],
                calc(x + 1, y, umask | (1 << (x + 1))));
      if (down)
        D = add(pow2[W * H - y * W - x - 1], calc(x + 1, y, smask));
      if (right)
        res = add(res, R);
      else if (down)
        res = add(res, D);
      else
        res = add(res, O);

      if (down)
        res = add(res, D);
      else if (right)
        res = add(res, R);
      else
        res = add(res, O);
    }
    return dp[x][y][mask] = res;
  }
  int sumUp(int _W, int _H, int _MOD) {
    MOD = _MOD;
    W = _W;
    H = _H;
    mst(dp, -1);
    pow2[0] = 1;
    repd(i, 1, W * H) pow2[i] = (ll)pow2[i - 1] * 2 % MOD;
    return calc(0, 0, 0);
  }
};
