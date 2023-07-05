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
int dx;
int mmp[500] = {0, 1, 1};
typedef set<int> SLL;
SLL ok;
priority_queue<int, vector<int>, greater<int>> q;
void init() {
  for (dx = 3; (mmp[dx] = mmp[dx - 1] + mmp[dx - 2]) < 1000000000; ++dx)
    ;
  ok.insert(0);
  ok.insert(1);
  q.push(1);
  int tp = 0;
  ll u;
  while (!q.empty()) {
    tp = q.top();
    q.pop();
    rep(i, 3, dx) {
      u = mmp[i] * (ll)tp;
      if (u > 1000000000ll)
        break;
      if (ok.find(u) == ok.end()) {
        ok.insert(u);
        q.push(u);
      }
    }
  }
}
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif
  init();
  int t;
  scanf("%d", &t);
  while (t--) {
    int n;
    scanf("%d", &n);
    if (ok.find(n) != ok.end())
      puts("Yes");
    else
      puts("No");
  }
  return 0;
}
