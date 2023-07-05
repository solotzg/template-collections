#include <cstdio>
#include <cstring>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;
#define rep(i, a, b) for (int i(a), __tzg##i(b); i < __tzg##i; ++i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, 1 + b)
#define urp(i, a, b) for (int i(a), __tzg##i(b); i >= __tzg##i; --i)
#define vrp(i, v) for (auto i(v.begin()); i != (v).end(); ++i)
int n, k;
int n0, n1;
int solve() {
  cin >> n >> k;
  n0 = n1 = 0;
  rp(i, n) {
    int a;
    cin >> a;
    if (a & 1)
      n1++;
    else
      ++n0;
  }
  if (k == n) {
    return n1 % 2 == 1;
  }
  int k0 = (n - k) / 2, k1 = (n - k) - k0;
  if (k0 >= n1)
    return 0;
  if (k0 >= n0) {
    if ((n1 - n0) % 2 == 1 && (k1 - k0) == 1)
      return 0;
    if ((n1 - n0) % 2 == 0 && (k1 - k0) == 0)
      return 0;
  } else if (k0 == k1)
    return 0;
  return 1;
}
int main() {
  if (!solve())
    puts("Daenerys");
  else
    puts("Stannis");
  return 0;
}
