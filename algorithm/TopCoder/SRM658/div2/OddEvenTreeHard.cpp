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
typedef vector<int> VI;
typedef pair<int, int> PII;
typedef vector<PII> VPII;
typedef long long ll;

const static int N = 55;

struct OddEvenTreeHard {
  int side[N];
  VI getTree(vector<string> x) {
    int n = x.size();
    rp(i, n) {
      if (x[i][i] == 'O')
        return {-1};
      else
        x[i][i] = 'E';
      rep(j, i + 1, n) {
        if (x[i][j] == '?') {
          if (x[j][i] != '?')
            x[i][j] = x[j][i];
        } else {
          if (x[j][i] == '?')
            x[j][i] = x[i][j];
          else if (x[j][i] != x[i][j])
            return {-1};
        }
      }
    }
    mst(side, -1);
    int s = 0, sd[2] = {0, 0};
    queue<int> q;
    rp(i, n) if (side[i] == -1) {
      rp(k, 2) if (sd[k] == 0) {
        s = k;
        break;
      }
      q.push(i);
      side[i] = s;
      sd[s] = 1;
      while (!q.empty()) {
        int o = q.front();
        q.pop();
        rp(j, n) {
          switch (x[o][j]) {
          case 'O':
            if (side[j] == -1) {
              side[j] = 1 - side[o];
              q.push(j);
              sd[side[j]] = 1;
            } else if (side[j] != 1 - side[o])
              return {-1};
            break;
          case 'E':
            if (side[j] == -1) {
              side[j] = side[o];
              q.push(j);
              sd[side[j]] = 1;
            } else if (side[j] != side[o])
              return {-1};
            break;
          }
        }
      }
    }
    VI res;
    int op = -1;
    rep(i, 1, n) if (side[i] != side[0]) res.pb(0), res.pb(i), op = i;
    if (op == -1)
      return {-1};
    rep(i, 1, n) if (side[i] == side[0]) res.pb(op), res.pb(i);
    return res;
  }
};

int main() {
  OddEvenTreeHard o;
  VI a = o.getTree({"?O??", "??O?", "???O", "????"});
  for (int p : a) {
    cout << p << endl;
  }
  return 0;
}
