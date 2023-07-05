#include <algorithm>
#include <cmath>
#include <cstdio>
#include <queue>
#include <vector>
#pragma comment(linker, "/STACK:1024000000,1024000000")
using namespace std;
const int MAXN = 100005;
typedef __int64 ll;
ll MOD = 1000000007;
ll num[MAXN];
void init() {
  num[0] = num[1] = 1;
  for (int i = 2; i <= 100000; ++i) {
    ll flg = 1;
    for (int k = 1;; ++k) {
      int a = i - (3 * k - 1) * k / 2, b = i - (3 * k + 1) * k / 2;
      ll s1, s2;
      if (a < 0)
        break;
      s1 = num[a];
      if (b < 0)
        s2 = 0;
      else
        s2 = num[b];
      num[i] = (num[i] + flg * (s1 + s2)) % MOD;
      flg = 0 - flg;
    }
    num[i] = (num[i] + MOD) % MOD;
  }
}
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif
  int t;
  scanf("%d", &t);
  init();
  while (t--) {
    int a, k;
    scanf("%d%d", &a, &k);
    ll res = num[a];
    for (int j = 1, f = -1;; ++j, f = 0 - f) {
      int c = (3 * j - 1) * j / 2 * k, d = (3 * j + 1) * j / 2 * k;
      ll s1, s2;
      if (c > a)
        break;
      s1 = f * num[a - c];
      if (d > a)
        s2 = 0;
      else
        s2 = f * num[a - d];
      res = (res + s1 + s2) % MOD;
    }
    res = (res + MOD) % MOD;
    printf("%I64d\n", res);
  }
  return 0;
}
