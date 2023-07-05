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
const double eps = 1e-11;
const double PI = acos(-1.0);
inline int sgn(double x) {
  if (fabs(x) < eps)
    return 0;
  if (x < 0)
    return -1;
  else
    return 1;
}
struct Point {
  double x, y;
  Point() {}
  Point(double _x, double _y) {
    x = _x;
    y = _y;
  }
  Point operator-(const Point &b) const { return Point(x - b.x, y - b.y); }
  double operator^(const Point &b) const { return x * b.y - y * b.x; }
  double operator*(const Point &b) const { return x * b.x + y * b.y; }
};
double dist(const Point &a, const Point &b) { return sqrt((a - b) * (a - b)); }
inline double cross(const Point &o, const Point &p1, const Point &p2) {
  return (p1 - o) ^ (p2 - o);
}
struct Flee {
  double maximalSafetyLevel(VI x, VI y) {
    int n = x.size();
    Point o(0, 0);
    vector<Point> l(n);
    rp(i, n) l[i] = Point(x[i], y[i]);
    double min_dis = 1e9;
    rp(i, n) min_dis = min(min_dis, dist(o, l[i]));
    if (n < 3)
      return min_dis;
    double m = 0.0;
    double s = abs(cross(l[0], l[1], l[2]));
    rp(i, n) {
      int ii = i, jj = (i + 1) % n;
      m += abs(cross(o, l[ii], l[jj]));
    }
    if (s && sgn(m - s) == 0) {
      double tp = -1e9;
      rp(i, n) rep(j, i + 1, n) {
        double d = dist(l[i], l[j]);
        tp = max(tp, d / 2.0);
      }
      min_dis = min(min_dis, tp);
    }
    return min_dis;
  }
};
int main() { return 0; }
