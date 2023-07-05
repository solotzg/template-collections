#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>
using namespace std;
#define rep(i, a, b) for (int i(a), __tzg##i(b); i < __tzg##i; ++i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, 1 + b)
#define urp(i, a, b) for (int i(a), __tzg##i(b); i >= __tzg##i; --i)
typedef long long ll;
const static int N = 100005;
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif
  int n;
  while (scanf("%d", &n) != EOF) {
    int cnt = 0;
    while (n) {
      ++cnt;
      n &= n - 1;
    }
    cout << (1 << cnt) << endl;
  }
  return 0;
}
