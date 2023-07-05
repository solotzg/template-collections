#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
typedef long long ll;
struct Prime {
  ll _maxn;
  int _sn;
  vector<int> _vis;
  vector<int> _prime;
  Prime(ll maxn) : _maxn(maxn) {
    _sn = (int)sqrt(_maxn) + 5;
    _vis.assign(_sn, 0);
    init_prime();
  }
  void init_prime() {
    for (int i = 2; i < _sn; ++i) {
      if (!_vis[i])
        _prime.push_back(i);
      for (int j = 0, np = _prime.size(); j < np && _prime[j] * i < _sn; ++j) {
        _vis[_prime[j] * i] = 1;
        if (i % _prime[j] == 0)
          break;
      }
    }
  }
  void divide(ll x, ll factor[], int cnt[], int &len) {
    len = 0;
    int n = 0;
    ll tp;
    for (int i = 0, np = _prime.size();
         i < np && (tp = _prime[i], tp * tp) <= x; ++i)
      if (x % tp == 0) {
        factor[len] = tp;
        n = 0;
        while (x % tp == 0)
          ++n, x /= tp;
        cnt[len++] = n;
      }
    if (x != 1ll) {
      factor[len] = x;
      cnt[len++] = 1;
    }
  }
} prime(10000000000ll);
const int N = 105;
ll factor[N], tar, res;
int cnt[N], len;
void dfs(int pos, ll val) {
  if (pos == len) {
    if (val > tar)
      res = min(res, val);
    return;
  }
  for (int i = 0; i <= cnt[pos]; ++i) {
    dfs(pos + 1, val);
    val *= factor[pos];
  }
}
struct Procrastination {
  const ll inf = 0x3f3f3f3f3f3f3f3f;
  ll get_next(ll a, ll b) {
    if (a >= b)
      return inf;
    prime.divide(b, factor, cnt, len);
    tar = a;
    res = inf;
    dfs(0, 1);
    return res;
  }
  long long findFinalAssignee(long long n) {
    for (ll a = 2; a * 2 <= n;) {
      if ((n - 1) % a == 0)
        n--;
      else if (n % a == 0)
        ++n;
      a = min(get_next(a, n - 1), get_next(a, n));
    }
    return n;
  }
};
int main() { return 0; }
