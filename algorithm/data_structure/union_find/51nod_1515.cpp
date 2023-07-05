#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
typedef long long ll;
#define N 100005
typedef unordered_set<int> ST;
unordered_map<int, ST> msp;
unordered_map<int, int> mp;
int find(int x) { return mp[x] = mp[x] == x ? x : find(mp[x]); }
int n, x, y, p;
int solve() {
  scanf("%d%d%d", &x, &y, &p);
  if (mp.find(x) == mp.end())
    mp[x] = x, msp[x] = ST();
  if (mp.find(y) == mp.end())
    mp[y] = y, msp[y] = ST();
  int ax = find(x), ay = find(y);
  if (p) {
    if (ax == ay)
      return 1;
    auto *sa = &msp[ax], *sb = &msp[ay];
    if (sa->size() > sb->size()) {
      swap(sa, sb);
      swap(ax, ay);
    }
    if (sa->find(ay) != sa->end())
      return 0;
    sb->insert(sa->begin(), sa->end());
    mp[ax] = ay;
    for (int u : *sa) {
      msp[u].erase(ax);
      msp[u].insert(ay);
    }
    return 1;
  } else {
    if (ax == ay)
      return 0;
    msp[ax].insert(ay);
    msp[ay].insert(ax);
    return 1;
  }
}
int main() {
  freopen("in.txt", "r", stdin);
  scanf("%d", &n);
  while (n--) {
    if (solve())
      puts("YES");
    else
      puts("NO");
  }
  return 0;
}
