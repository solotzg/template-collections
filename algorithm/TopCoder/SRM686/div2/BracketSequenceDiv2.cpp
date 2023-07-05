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

const int N = 105;
int mmp[N][N], MOD = 1e9 + 7;
string s;
inline int mul(int a, int b) { return (ll)a * b % MOD; }
inline int add(int a, int b) { return (a + b) % MOD; }
int n;
int dp(int idx, int pre) {
  if (idx == n)
    return pre == 0;
  if (mmp[idx][pre] != -1)
    return mmp[idx][pre];
  int &res = mmp[idx][pre];
  res = pre == 0;
  repd(i, idx, n - 1) if (s[i] == '(') {
    res = add(res, dp(i + 1, pre + 1));
    break;
  }
  if (pre)
    repd(i, idx, n - 1) if (s[i] == ')') {
      res = add(res, dp(i + 1, pre - 1));
      break;
    }
  return res;
}
struct BracketSequenceDiv2 {
  int count(string _s) {
    s = _s;
    n = s.size();
    mst(mmp, -1);
    int o = dp(0, 0);
    return (o - 1 + MOD) % MOD;
  }
};

int main() {
#ifdef _TZG_DEBUG
  freopen("in.txt", "r", stdin);
#endif // _TZG_DEBUG
  BracketSequenceDiv2 b;
  string p = "()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()("
             ")()()()()()()()()()()()()()()()()()";
  cout << p << endl;
  cout << b.count(p) << endl;
  return 0;
}
