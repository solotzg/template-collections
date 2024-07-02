#pragma once

#include <utils/utils.h>

namespace geometry_2d {

constexpr double eps = 1e-8;
static const double PI = std::acos(-1.0);

inline int sgn(double x) {
  if (std::fabs(x) < eps)
    return 0;
  if (x < 0)
    return -1;
  else
    return 1;
}

inline int cmp(double x, double y) { return sgn(x - y); }

struct Point;
using Vec = Point;

struct Point {
  double x, y;
  Point() = default;
  Point(double _x, double _y) {
    x = _x;
    y = _y;
  }
  Vec operator-(const Point &b) const { return Point(x - b.x, y - b.y); }
  Vec operator+(const Point &b) const { return Point(x + b.x, y + b.y); }
  Point operator*(double v) const { return Point(x * v, y * v); }
  Point operator/(double v) const { return Point(x / v, y / v); }
  Point &operator*=(double v) {
    x *= v;
    y *= v;
    return *this;
  }
  Point &operator/=(double v) {
    x /= v;
    y /= v;
    return *this;
  }
  /*
  Dot Product
  a · b = |a| × |b| × cos(θ)
  |a| is the magnitude (length) of vector a
  |b| is the magnitude (length) of vector b
  θ is the angle (counterclockwise) between a and b
  */
  double dot(const Point &b) const { return x * b.x + y * b.y; }
  double operator*(const Point &b) const { return dot(b); }

  bool operator==(const Point &b) const {
    return sgn(x - b.x) == 0 && sgn(y - b.y) == 0;
  }

  /*
  Cross Product
  a × b = |a| |b| sin(θ) n
  |a| is the magnitude (length) of vector a
  |b| is the magnitude (length) of vector b
  θ is the angle (counterclockwise) from a to b
  n is the unit vector at right angles to both a and b
  */
  double cross(const Point &p) const { return x * p.y - y * p.x; }
  double operator^(const Point &b) const { return cross(b); }
  double dist(const Point &p) const { return (*this - p).len(); }
  double len() const { return sqrt(len_pow()); }
  double len_pow() const { return (*this) * (*this); }
};

inline bool CmpLessPoint(const Point &p1, const Point &p2) {
  if (p1.x != p2.x) {
    return p1.x < p2.x;
  }
  return p1.y < p2.y;
}

// v1 counterclockwise rotate to v2: return true
// same director: return v1.len() < v2.len()
inline bool CmpLessVec(const Vec &v1, const Vec &v2) {
  if (auto f = sgn(v1.cross(v2)); f > 0) {
    return true;
  } else if (f == 0 && sgn(v1.len_pow() - v2.len_pow()) < 0) {
    return true;
  }
  return false;
}

inline double Cross(const Point &o, const Point &p1, const Point &p2) {
  return (p1 - o) ^ (p2 - o);
}

struct Line {
  Point s;
  Point e;
  Line() = default;
  Line(const Point &_s, const Point &_e) : s(_s), e(_e) { assert(s != e); }
};

using Segment = Line;

// Ax + By + C = 0
inline void Coefficient(const Line &L, double &A, double &B, double &C) {
  A = L.e.y - L.s.y;
  B = L.s.x - L.e.x;
  C = L.e.x * L.s.y - L.s.x * L.e.y;
}

// Intersection of two lines
struct Intersection {
  enum class State { SameLine, Parallelism, Intersection };
  std::pair<State, Point> operator()(const Line &A, const Line &B) {
    double A1, B1, C1;
    double A2, B2, C2;
    Coefficient(A, A1, B1, C1);
    Coefficient(B, A2, B2, C2);
    Point I(0, 0);
    if (sgn(A1 * B2 - A2 * B1) == 0) {
      if (sgn(A1 * C2 - A2 * C1) == 0 && sgn(B1 * C2 - B2 * C1) == 0)
        return std::make_pair(State::SameLine, I);
      return std::make_pair(State::Parallelism, I);
    }
    I.x = -(B2 * C1 - B1 * C2) / (A1 * B2 - A2 * B1);
    I.y = (A2 * C1 - A1 * C2) / (A1 * B2 - A2 * B1);
    return std::make_pair(State::Intersection, I);
  }
};

// Counterclockwise rotation Angle(radian value)
inline Vec Rotate(const Vec &A, double rad) {
  return Vec(A.x * std::cos(rad) - A.y * std::sin(rad),
             A.x * std::sin(rad) + A.y * std::cos(rad));
}

// Distance between two points
inline double Dist(const Point &a, const Point &b) { return a.dist(b); }

// Determine the intersection of line segments
inline bool IsSegInterSeg(const Segment &l1, const Segment &l2) {
  return std::max(l1.s.x, l1.e.x) >= std::min(l2.s.x, l2.e.x) &&
         std::max(l2.s.x, l2.e.x) >= std::min(l1.s.x, l1.e.x) &&
         std::max(l1.s.y, l1.e.y) >= std::min(l2.s.y, l2.e.y) &&
         std::max(l2.s.y, l2.e.y) >= std::min(l1.s.y, l1.e.y) &&
         sgn((l2.s - l1.e) ^ (l1.s - l1.e)) *
                 sgn((l2.e - l1.e) ^ (l1.s - l1.e)) <=
             0 &&
         sgn((l1.s - l2.e) ^ (l2.s - l2.e)) *
                 sgn((l1.e - l2.e) ^ (l2.s - l2.e)) <=
             0;
}

// Determine the intersection of a line and a segment
inline bool IsSegInterLine(const Segment &seg, const Line &line) {
  return sgn((seg.s - line.e) ^ (line.s - line.e)) *
             sgn((seg.e - line.e) ^ (line.s - line.e)) <=
         0;
}

// Distance from point to line
// Get the point to the nearest point on the line
inline double PointToLine(const Point &P, const Line &L, Point &res) {
  double d = Dist(L.s, L.e);
  double s = ((L.s - P) ^ (L.e - P)) / d;
  res.x = P.x + s * (L.e.y - L.s.y) / d;
  res.y = P.y - s * (L.e.x - L.s.x) / d;
  return abs(s);
}

// Distance from point to line segment
// Get the point to the nearest point of the line segment
inline Point NearestPointToLineSeg(const Point &P, const Segment &seg) {
  Point result;
  double t =
      ((P - seg.s) * (seg.e - seg.s)) / ((seg.e - seg.s) * (seg.e - seg.s));
  if (t >= 0 && t <= 1) {
    result.x = seg.s.x + (seg.e.x - seg.s.x) * t;
    result.y = seg.s.y + (seg.e.y - seg.s.y) * t;
  } else {
    if ((P - seg.s).len_pow() < (P - seg.e).len_pow())
      result = seg.s;
    else
      result = seg.e;
  }
  return result;
}

// Calculated polygon area
inline double CalcArea(const Point *p, int n) {
  double res = 0;
  for (int i = 0; i < n; i++)
    res += (p[i] ^ p[(i + 1) % n]);
  return std::fabs(res / 2.0);
}

// Check whether the point on the line segment
inline bool IsPointOnSeg(const Point &P, const Segment &seg) {
  return sgn((seg.s - P) ^ (seg.e - P)) == 0 &&
         cmp(P.x, std::min(seg.s.x, seg.e.x)) >= 0 &&
         cmp(P.x, std::max(seg.s.x, seg.e.x)) <= 0 &&
         cmp(P.y, std::min(seg.s.y, seg.e.y)) >= 0 &&
         cmp(P.y, std::max(seg.s.y, seg.e.y)) <= 0 &&
         //
         true;
}

// Determine whether the point is inside a convex polygon
// The points form a convex hull and are sorted counterclockwise
/*
Return value:
  -1: The point is outside the convex polygon
  0: The point is on the convex polygon boundary
  1: The point is inside the convex polygon
*/
inline int IsInConvexPoly(const Point &a, const Point *p, int n) {
  for (int i = 0; i < n; i++) {
    if (sgn((p[i] - a) ^ (p[(i + 1) % n] - a)) < 0)
      return -1;
    else if (IsPointOnSeg(a, Line(p[i], p[(i + 1) % n])))
      return 0;
  }
  return 1;
}

// Determines whether the point is inside any polygon by `Ray` method
/*
Return value:
  -1: The point is outside the convex polygon
  0: The point is on the convex polygon boundary
  1: The point is inside the convex polygon
*/
inline int InPoly(const Point &p, const Point *poly, int n) {
  ASSERT_GE(n, 3);

  int cnt;
  Segment ray, side;
  cnt = 0;
  ray.s = p;
  ray.e.y = p.y;
  ray.e.x = poly[0].x;
  rep(i, 1, n) { ray.e.x = std::min(ray.e.x, poly[i].x); }
  ray.e.x -= 1;

  for (int i = 0; i < n; i++) {
    side.s = poly[i];
    side.e = poly[(i + 1) % n];
    if (IsPointOnSeg(p, side))
      return 0;
    // ignore parallel
    if (sgn(side.s.y - side.e.y) == 0)
      continue;
    if (IsPointOnSeg(side.s, ray)) {
      if (sgn(side.s.y - side.e.y) > 0)
        cnt++;
    } else if (IsPointOnSeg(side.e, ray)) {
      if (sgn(side.e.y - side.s.y) > 0)
        cnt++;
    } else if (IsSegInterSeg(ray, side))
      cnt++;
  }
  if (cnt % 2 == 1)
    return 1;
  else
    return -1;
}

// Check whether convex polygon
inline bool IsConvex(const Point *poly, int n) {
  ASSERT_GE(n, 3);

  bool s[3];
  std::memset(s, false, sizeof(s));
  for (int i = 0; i < n; i++) {
    s[sgn((poly[(i + 1) % n] - poly[i]) ^ (poly[(i + 2) % n] - poly[i])) + 1] =
        true;
    if (s[0] && s[2])
      return false;
  }
  return true;
}

// Convex hull, Graham algorithm
struct Graham {
  std::vector<Point> operator()(const Point *data, int n) {
    if (n == 0) {
      return {};
    }

    std::vector<Point> stack;
    stack.resize(n);
    for (int i = 0; i < n; ++i)
      stack[i] = data[i];

    {
      int k = 0;
      auto p0 = stack[0];
      for (int i = 1; i < n; i++) {
        if ((p0.y > stack[i].y) || (p0.y == stack[i].y && p0.x > stack[i].x)) {
          p0 = stack[i];
          k = i;
        }
      }
      std::swap(stack[k], stack[0]);
    }

    std::sort(stack.begin() + 1, stack.end(),
              [&stack](const Point &p1, const Point &p2) {
                return CmpLessVec(p1 - stack[0], p2 - stack[0]);
              });

    if (n == 1) {
      return stack;
    }

    if (n == 2) {
      return stack;
    }

    int top = 2;
    for (int i = 2; i < n; i++) {
      while (top > 1 && sgn((stack[top - 1] - stack[top - 2]) ^
                            (stack[i] - stack[top - 2])) <= 0)
        top--;
      stack[top++] = stack[i];
    }

    stack.resize(top);
    return stack;
  }
};

// Center of circle
inline Point CenterOfCircle(const Point &a, const Point &b, const Point &c) {
  double a1 = b.x - a.x, b1 = b.y - a.y, c1 = (a1 * a1 + b1 * b1) / 2;
  double a2 = c.x - a.x, b2 = c.y - a.y, c2 = (a2 * a2 + b2 * b2) / 2;
  double d = a1 * b2 - a2 * b1;
  return Point(a.x + (c1 * b2 - c2 * b1) / d, a.y + (a1 * c2 - a2 * c1) / d);
}

// Common area of the two circles
inline double AreaOfOverlap(const Point &c1, double r1, const Point &c2,
                            double r2) {
  double d = Dist(c1, c2);
  if (r1 + r2 < d + eps)
    return 0;
  if (d < std::fabs(r1 - r2) + eps) {
    double r = std::min(r1, r2);
    return PI * r * r;
  }
  double x = (d * d + r1 * r1 - r2 * r2) / (2 * d);
  double t1 = std::acos(x / r1);
  double t2 = std::acos((d - x) / r2);
  return r1 * r1 * t1 + r2 * r2 * t2 - d * r1 * std::sin(t1);
}

// sort point round center counterclockwisely
template <typename It, typename FIntoPoint>
inline std::vector<double> SortPoints(const Point &center, It begin, It end,
                                      FIntoPoint &&func_to_point,
                                      size_t &center_same_cnt) {
  std::vector<double> degrees;
  degrees.reserve(end - begin);
  center_same_cnt = 0;

  for (auto it = begin; it != end; it += 1) {
    auto &&p = *it;
    Vec v = func_to_point(p) - center;
    if (v.x == 0 && v.y == 0) {
      center_same_cnt++;
    } else {
      double o = std::atan2(v.y, v.x);
      if (sgn(o) < 0) {
        o += PI * 2;
      }
      degrees.emplace_back(o);
    }
  }
  int n = degrees.size();
  std::sort(degrees.begin(), degrees.end());

  if (n) {
    double _d = degrees[n - 1];
    for (int i = n - 1; i >= 1; i--) {
      degrees[i] = degrees[i] - degrees[i - 1];
    }
    degrees[0] = degrees[0] + 2 * PI - _d;
    for (int i = 0; i < n; ++i) {
      if (sgn(degrees[i]) == 0)
        degrees[i] = 0;
    }
  }

  return degrees;
}
} // namespace geometry_2d
