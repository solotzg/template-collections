#include <algorithm>
#include <cstdio>
#include <cstring>
using namespace std;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
typedef long long ll;
ll cmp(ll s1, ll v1, ll s2, ll v2) { return v1 * s2 - v2 * s1; }
ll gcd(ll a, ll b) { return b ? gcd(b, a % b) : a; }
ll solve(ll w, ll s1, ll v1, ll s2, ll v2) {
  ll g = gcd(s1, s2);
  ll lcm = s1 / g * s2;
  ll k = w / lcm;
  w -= k * lcm;
  if (k)
    --k, w += lcm;
  ll k1 = k * (lcm / s1);
  ll sum = k1 * v1, res = 0;
  if (s1 < s2)
    swap(s1, s2), swap(v1, v2);
  for (k1 = 0; k1 <= w / s1; ++k1) {
    ll k2 = (w - s1 * k1) / s2;
    res = max(res, k1 * v1 + k2 * v2);
  }
  return res + sum;
}
ll calc(ll w, ll s1, ll v1, ll s2, ll v2) {
  if (cmp(s1, v1, s2, v2) < 0)
    return solve(w, s2, v2, s1, v1);
  else
    return solve(w, s1, v1, s2, v2);
}
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif
  int t;
  ll w, s1, v1, s2, v2;
  scanf("%d", &t);
  for (int i = 1; i <= t; ++i) {
    printf("Case #%d: ", i);
    scanf("%lld%lld%lld%lld%lld", &w, &s1, &v1, &s2, &v2);
    printf("%lld\n", calc(w, s1, v1, s2, v2));
  }
  return 0;
}
