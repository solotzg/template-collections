/*
    记忆化搜索
    状态：dp[当前要处理的位][x1 x2 间已存在的1的个数][x1>=x2][x1
   x2和原始数的关系]
*/

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
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

const int N = 1000, MOD = 998244353;
char s[N];
inline int add(int a, int b) {
  a += b;
  return a >= MOD ? a - MOD : a;
}
inline int mul(int a, int b) { return (ll)a * b % MOD; }
int len10, len2;
int num10[N], num2[N];
int dp[N][N * 2][2][2], gg;
int divide() {
  int s = 0;
  urp(i, len10 - 1, 0) {
    s = s * 10 + num10[i];
    num10[i] = s / 2;
    if (s)
      gg = 1;
    s %= 2;
  }
  return s;
}
int fuc(int dx, int diff, int cmp, int cmp_all) {
  if (dx == len2) {
    return (cmp == 1 && diff < 0);
  }
  int &d = dp[dx][diff + N][cmp][cmp_all];
  if (d != -1)
    return d;
  d = 0;
  int tmp;
  rp(x1, 2) rp(x2, 2) {
    tmp = x1 - x2 + diff;
    if (cmp == 1) {
      if (cmp_all == 1) {
        d = add(d, fuc(dx + 1, tmp, cmp, cmp_all));
      } else {
        if (x1 == num2[dx])
          d = add(d, fuc(dx + 1, tmp, cmp, 0));
        else if (x1 < num2[dx])
          d = add(d, fuc(dx + 1, tmp, cmp, 1));
      }
    } else {
      if (x1 < x2)
        continue;
      if (cmp_all == 1) {
        d = add(d, fuc(dx + 1, tmp, x1 == x2 ? 0 : 1, 1));
      } else {
        if (x1 > x2) {
          if (x1 < num2[dx])
            d = add(d, fuc(dx + 1, tmp, 1, 1));
          else if (x1 == num2[dx])
            d = add(d, fuc(dx + 1, tmp, 1, 0));
        } else {
          if (x1 < num2[dx])
            d = add(d, fuc(dx + 1, tmp, 0, 1));
          else if (x1 == num2[dx])
            d = add(d, fuc(dx + 1, tmp, 0, 0));
        }
      }
    }
  }
  //
  return d;
}
int calc() {
  len10 = strlen(s);
  rp(i, len10) num10[i] = s[len10 - 1 - i] - '0';
  len2 = 0;
  while (1) {
    gg = 0;
    int r = divide();
    if (!gg)
      break;
    num2[len2++] = r;
  }
  reverse(num2, num2 + len2);
  mst(dp, -1);
  return fuc(0, 0, 0, 0);
}
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif // ONLINE_JUDGE
  int t;
  cin >> t;
  while (t--) {
    scanf("%s", s);
    printf("%d\n", calc());
  }
  return 0;
}
