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

struct ParenthesesDiv1Easy {
  const static int N = 1005;
  int stk[N];
  VI correct(string s) {
    int n = s.size();
    if (n & 1)
      return {-1};
    int top = 0;
    rp(i, n) {
      if (s[i] == '(') {
        stk[top++] = i;
      } else {
        if (top == 0 || s[stk[top - 1]] == ')')
          stk[top++] = i;
        else
          --top;
      }
    }
    VI res;
    if (top == 0)
      return {};
    int t = s[stk[0]] == '(';
    int m = -1;
    rp(i, top) {
      if (s[stk[i]] == '(') {
        if (t == 0)
          t = 2, m = i;
      }
    }
    if (t == 0) {
      res.pb(0);
      res.pb(stk[top / 2 - 1]);
      return res;
    } else if (t == 1) {
      res.pb(stk[top / 2]);
      res.pb(n - 1);
      return res;
    } else {
      if (m <= top / 2) {
        res.pb(0);
        res.pb(stk[m - 1]);
        res.pb(stk[top / 2]);
        res.pb(n - 1);
        return res;
      } else {
        res.pb(0);
        res.pb(stk[top / 2 - 1]);
        res.pb(stk[m]);
        res.pb(n - 1);
        return res;
      }
    }
    return {};
  }
};
