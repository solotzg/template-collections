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
struct RedAndBluePoints {
  int find(VI blueX, VI blueY, VI redX, VI redY);
};
struct Point {
  int x, y;
  Point() {}
  Point(int _x, int _y) {
    x = _x;
    y = _y;
  }
  Point operator-(const Point &b) const { return Point(x - b.x, y - b.y); }
  // 叉积
  int operator^(const Point &b) const { return x * b.y - y * b.x; }
  bool operator<(const Point &b) const {
    return y == b.y ? (x < b.x) : (y < b.y);
  }
};

inline int cross(const Point &o, const Point &p1, const Point &p2) {
  return (p1 - o) ^ (p2 - o);
}

int RedAndBluePoints::find(VI blueX, VI blueY, VI redX, VI redY) {
  vector<Point> vb, vr;
  int nb = blueY.size(), nr = redY.size();
  rp(i, nb) vb.pb(Point(blueX[i], blueY[i]));
  rp(i, nr) vr.pb(Point(redX[i], redY[i]));
  sort(vtr(vb));
  int res = 1;
  static int dp[55][55];
  /*
    for (Point & u : vb) {
    printf("%d, %d\n", u.x, u.y);
    }
    puts("_________");
  */
  rp(start, nb) {
    vector<Point> tpb(vb.begin() + start, vb.end());
    const Point &o = tpb[0];
    auto cmp = [&](const Point &p1, const Point &p2) {
      return cross(o, p1, p2) > 0;
    };
    sort(tpb.begin() + 1, tpb.end(), cmp);
    int sb = tpb.size();
    /*
      for (Point & u : tpb) {
      printf("%d, %d\n", u.x, u.y);
      }
      puts("..........");
    */
    mst(dp, -1);
    rep(i, 1, sb) { dp[0][i] = 2; }
    rp(i, sb) rep(j, i + 1, sb) {
      if (dp[i][j] == -1)
        continue;
      res = max(res, dp[i][j]);
      rep(k, j + 1, sb) {
        if (cross(tpb[i], tpb[j], tpb[k]) < 0)
          continue;
        int f = 1, s = 0;
        rp(t, nr) {
          if ((cross(o, tpb[j], vr[t]) > 0) &&
              (cross(tpb[j], tpb[k], vr[t]) > 0) &&
              (cross(tpb[k], o, vr[t]) > 0)) {
            f = 0;
            break;
          }
        }
        if (!f)
          continue;
        rep(t, j + 1, k) {
          if (cross(tpb[j], tpb[k], tpb[t]) > 0)
            ++s;
        }
        dp[j][k] = max(dp[j][k], dp[i][j] + s + 1);
        // printf("%d,%d -> %d,%d ==> %d\n", i,j, j,k, dp[j][k]);
      }
    }
  }
  /*
    printf("%d\n", res);
  */
  return res;
}
int main() {
  auto t = new RedAndBluePoints;
  assert(t->find({0, 0, 10, 10}, {0, 10, 0, 10}, {100}, {120}) == 4);
  assert(
      t->find({467, 873, 889, 149, 725, 142, 473, 326, 661, 307, 391, 92,  89,
               30,  625, 604, 671, 547, 740, 875, 277, 107, 336, 124, 717, 521,
               10,  275, 396, 474, 922, 48,  6,   470, 97,  31,  646, 209, 680,
               385, 185, 277, 835, 395, 207, 658, 774, 239, 68,  439},
              {768, 51,  641, 714, 828, 852, 220, 24,  451, 368, 4,   384, 554,
               136, 339, 279, 272, 516, 900, 590, 701, 162, 192, 792, 90,  734,
               51,  373, 517, 331, 839, 464, 52,  743, 869, 19,  928, 488, 514,
               823, 377, 115, 706, 506, 565, 697, 31,  918, 52,  624},
              {702, 180, 907, 836, 153, 441, 223, 163, 768, 108, 735, 894, 462,
               400, 303, 710, 254, 980, 818, 926, 372, 289, 493, 161, 132, 271,
               797, 486, 24,  378, 59,  790, 945, 904, 894, 33,  989, 855, 507,
               614, 201, 328, 35,  104, 484, 727, 834, 240, 311, 502},
              {223, 290, 834, 8,   233, 654, 621, 173, 124, 344, 840, 617, 288,
               869, 529, 375, 949, 701, 690, 698, 382, 264, 502, 989, 883, 111,
               63,  704, 751, 376, 174, 359, 276, 986, 87,  761, 55,  337, 435,
               587, 323, 30,  224, 163, 619, 115, 980, 451, 577, 996}) == 11);
  assert(
      t->find({291, 469, 468, 164, 96,  135, 44,  505, 331, 504, 745, 987, 762,
               413, 162, 77,  551, 622, 474, 867, 180, 175, 566, 132, 94,  328,
               234, 267, 246, 760, 949, 512, 806, 551, 919, 594, 256, 1,   333},
              {985, 807, 420, 443, 482, 966, 645, 981, 578, 860, 589, 782, 672,
               58,  831, 950, 938, 359, 7,   633, 793, 427, 231, 827, 288, 618,
               772, 277, 559, 929, 495, 720, 622, 27,  924, 249, 505, 879, 461},
              {14, 81, 596, 96, 816, 883, 802, 874, 122, 963, 52, 873, 47, 694},
              {326, 639, 102, 245, 909, 127, 242, 990, 617, 514, 701, 385, 301,
               348}) == 31);
  return 0;
}
