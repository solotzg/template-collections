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

struct FindingKids {
  const static int M = 1e9 + 7, N = 2e5 + 5;
  int lpos[N], rpos[N], bpos[N];
  int nl, nr;
  int t;
  PII pos[N];
  ll getSum(int n, int q, int a, int b, int c) {
    ll A, B, C;
    A = a;
    B = b;
    C = c;
    nl = nr = 0;
    unordered_set<int> s;
    rp(i, n) {
      A = (A * B + C) % M;
      int p = A % (M - n + i + 1);
      if (s.find(p) != s.end())
        p = M - n + i;
      pos[i] = mp(p, i);
      if (p % 2 == 0)
        rpos[nr++] = p;
      else
        lpos[nl++] = p;
      s.insert(p);
    }
    sort(lpos, lpos + nl);
    sort(rpos, rpos + nr);
    sort(pos, pos + n);
    rp(i, n) bpos[pos[i].se] = i;
    ll res = 0;
    rp(i, q) {
      A = (A * B + C) % M;
      int kid = A % n;
      A = (A * B + C) % M;
      t = A;
      kid = bpos[kid];
      int bg = -M, ed = M * 2 + 5;
      ll len = (ll)ed - bg;
      while (len > 0) {
        int half = len / 2;
        int mid = bg + half;
        int cl = upper_bound(lpos, lpos + nl, (ll)mid + t) - lpos;
        int cr = upper_bound(rpos, rpos + nr, (ll)mid - t) - rpos;
        if (cl + cr >= kid + 1)
          ed = mid;
        else
          bg = mid + 1;
        len = (ll)ed - bg;
      }
      res += abs(bg);
    }
    return res;
  }
};
int main() {
  FindingKids *k = new FindingKids;
  cout << k->getSum(200000, 200000, 12345, 67890, 111213141

                    )
       << endl;
  return 0;
}
