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

const int N = 50000 + 5;
ll sub_sum[N];
int lf[N], rt[N];
void rm(int pos) {
  int l = lf[pos], r = rt[pos];
  if (l)
    rt[l] = r;
  if (r)
    lf[r] = l;
}
int main() {
#ifdef _TZG_DEBUG
  freopen("in.txt", "r", stdin);
#endif // _TZG_DEBUG
  int n, m, cnt = 0, cm = 0;
  cin >> n >> m;
  ll res = 0ll, sum = 0ll;
  set<pair<ll, int>> s;
  rp(i, n) {
    int x;
    scanf("%d", &x);
    if ((sum < 0 && x > 0) || (sum > 0 && x < 0)) {
      sub_sum[++cnt] = sum;
      s.insert(mp(abs(sum), cnt));
      cm += sum > 0;
      sum = 0;
    }
    sum += x;
    res += x > 0 ? x : 0;
  }
  sub_sum[++cnt] = sum;
  s.insert(mp(abs(sum), cnt));
  cm += sum > 0;
  repd(i, 1, cnt) {
    lf[i] = i - 1;
    rt[i] = i + 1;
  }
  rt[cnt] = 0;
  while (cm > m) {
    auto it = s.begin();
    auto p = *it;
    s.erase(it);
    ll a;
    int pos = p.se;
    a = sub_sum[pos];
    if (a < 0 && (_0(lf[pos]) || _0(rt[pos])) || _0(a))
      continue;
    res -= abs(a);
    s.erase(mp(abs(sub_sum[lf[pos]]), lf[pos]));
    s.erase(mp(abs(sub_sum[rt[pos]]), rt[pos]));
    sub_sum[pos] = sub_sum[lf[pos]] + a + sub_sum[rt[pos]];
    s.insert(mp(abs(sub_sum[pos]), pos));
    rm(lf[pos]);
    rm(rt[pos]);
    --cm;
  }
  cout << res << endl;
  return 0;
}
