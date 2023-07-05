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
struct FoxConnection3 {
  long long minimalSteps(vector<int> _x, vector<int> _y);
};

const int N = 20;
ll x[N], y[N], res;
int dx[] = {0, 0, 1, -1};
int dy[] = {1, -1, 0, 0};
int xx[N], yy[N];
int n, pp[N];
set<ll> sset;
ll m_hash(int ex[], int ey[]) {
  ll r = 0;
  rp(i, n) r = r * 20 + (ex[i] + 10);
  rp(i, n) r = r * 20 + (ey[i] + 10);
  return r;
}
void solve() {
  static int cpx[N], cpy[N];
  static ll vx[N], vy[N];
  memcpy(cpx, xx, sizeof xx);
  memcpy(cpy, yy, sizeof yy);
  // sort cpx, cpy
  rp(i, n) rep(j, i + 1,
               n) if (cpx[i] > cpx[j] || (cpx[i] == cpx[j] && cpy[i] > cpy[j]))
      swap(cpx[i], cpx[j]),
      swap(cpy[i], cpy[j]);
  urp(i, n - 1, 0) cpx[i] -= cpx[0], cpy[i] -= cpy[0];
  ll vhash = m_hash(cpx, cpy);
  if (sset.find(vhash) != sset.end())
    return;
  sset.insert(vhash);
  rp(i, n) pp[i] = i;
  do {
    rp(i, n) vx[i] = x[i] - cpx[pp[i]], vy[i] = y[i] - cpy[pp[i]];
    sort(vx, vx + n);
    sort(vy, vy + n);
    ll sum = 0ll;
    rp(i, n) sum += abs(vx[i] - vx[n / 2]);
    rp(i, n) sum += abs(vy[i] - vy[n / 2]);
    if (sum < res) {
      res = sum;
    }
  } while (next_permutation(pp, pp + n));
}
void dfs(int idx) {
  if (idx == n - 1) {
    solve();
    return;
  }
  int nx, ny, f;
  repd(i, 0, idx) rp(j, 4) {
    nx = xx[i] + dx[j];
    ny = yy[i] + dy[j];
    f = 1;
    repd(k, 0, idx) if (xx[k] == nx && yy[k] == ny) {
      f = 0;
      break;
    }
    if (!f)
      continue;
    xx[idx + 1] = nx;
    yy[idx + 1] = ny;
    dfs(idx + 1);
  }
}
ll FoxConnection3::minimalSteps(vector<int> _x, vector<int> _y) {
  n = _x.size();
  res = 0x3f3f3f3f3f3f3f3f;
  rp(i, n) x[i] = _x[i], y[i] = _y[i];
  xx[0] = yy[0] = 0;
  dfs(0);
  return res;
}
