#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
#include<assert.h>
using namespace std;
const double eps = 1e-8;
const double PI = acos(-1.0);
int sgn(double x) {
    if(fabs(x) < eps)return 0;
    if(x < 0)return -1;
    else return 1;
}
struct Point {
    double x,y;
    Point() {}
    Point(double _x, double _y) {
        x = _x;
        y = _y;
    }
    Point operator - (const Point &b)const {
        return Point(x - b.x,y - b.y);
    }
//叉积
    double operator ^ (const Point &b)const {
        return x*b.y - y*b.x;
    }
//点积
    double operator * (const Point &b)const {
        return x*b.x + y*b.y;
    }
    bool operator == (const Point & b) const {
        return sgn(x-b.x)==0 && sgn(y-b.y)==0;
    }
};

struct Line {
    Point s;
    Point e;
    Line() {}
    Line(Point _s, Point _e) {
        s = _s;
        e = _e;
        assert(!(s == e));
    }
};
// 返回直线 Ax + By + C =0 的系数
void Coefficient(const Line & L, double & A, double & B, double & C) {
    A = L.e.y - L.s.y;
    B = L.s.x - L.e.x;
    C = L.e.x * L.s.y - L.s.x * L.e.y;
}
//两直线相交求交点
//第一个值为0表示直线重合,为1表示平行,为2是相交
//只有第一个值为2时,交点才有意义
pair<int,Point> Intersection(const Line & A, const Line & B) {
    double A1, B1, C1;
    double A2, B2, C2;
    Coefficient(A, A1, B1, C1);
    Coefficient(B, A2, B2, C2);
    Point I(0, 0);
    if (sgn(A1 * B2 - A2 * B1) == 0) {
        if (sgn(A1 * C2 - A2 * C1)==0 && sgn(B1 * C2 - B2 * C1)==0)
            return make_pair(0, I);
        return make_pair(1, I);
    }
    I.x = - (B2 * C1 - B1 * C2) / (A1 * B2 - A2 * B1);
    I.y = (A2 * C1 - A1 * C2) / (A1 * B2 - A2 * B1);
    return make_pair(2, I);
}
// 绕原点逆时针旋转角度B(弧度值)
Point rotate(Point A, double rad) {
    return Point (A.x * cos (rad) - A.y * sin (rad), A.x * sin (rad) + A.y * cos (rad));
}
//*两点间距离
double dist(Point a,Point b) {
    return sqrt((a-b)*(a-b));
}
//*判断线段相交
bool inter(Line l1,Line l2) {
    return
        max(l1.s.x,l1.e.x) >= min(l2.s.x,l2.e.x) &&
        max(l2.s.x,l2.e.x) >= min(l1.s.x,l1.e.x) &&
        max(l1.s.y,l1.e.y) >= min(l2.s.y,l2.e.y) &&
        max(l2.s.y,l2.e.y) >= min(l1.s.y,l1.e.y) &&
        sgn((l2.s-l1.e)^(l1.s-l1.e))*sgn((l2.e-l1.e)^(l1.s-l1.e)) <= 0 &&
        sgn((l1.s-l2.e)^(l2.s-l2.e))*sgn((l1.e-l2.e)^(l2.s-l2.e)) <= 0;
}
//判断直线和线段相交
bool Seg_inter_line(Line l1,Line l2) { //判断直线l1和线段l2是否相交
    return sgn((l2.s-l1.e)^(l1.s-l1.e))*sgn((l2.e-l1.e)^(l1.s-l1.e)) <= 0;
}
//点到直线距离
//返回为result,是点到直线最近的点
double PointToLine(Point P,Line L,Point & res) {
    double d = dist(L.s, L.e);
    double s = ((L.s-P)^(L.e-P))/d;
    res.x = P.x + s*(L.e.y-L.s.y)/d;
    res.y = P.y - s*(L.e.x-L.s.x)/d;
    return abs(s);
}
//点到线段的距离
//返回点到线段最近的点
Point NearestPointToLineSeg(Point P,Line L) {
    Point result;
    double t = ((P-L.s)*(L.e-L.s))/((L.e-L.s)*(L.e-L.s));
    if(t >= 0 && t <= 1) {
        result.x = L.s.x + (L.e.x - L.s.x)*t;
        result.y = L.s.y + (L.e.y - L.s.y)*t;
    } else {
        if(dist(P,L.s) < dist(P,L.e))
            result = L.s;
        else result = L.e;
    }
    return result;
}
//计算多边形面积
//点的编号从0~n-1
double CalcArea(Point p[],int n) {
    double res = 0;
    for(int i = 0; i < n; i++)
        res += (p[i]^p[(i+1)%n]);
    return fabs(res/2.0);
}
//*判断点在线段上
bool OnSeg(Point P,Line L) {
    return
        sgn((L.s-P)^(L.e-P)) == 0 &&
        (((P.x - L.s.x) * (P.x - L.e.x) < 0)  ||
         ((P.y - L.s.y) * (P.y - L.e.y) < 0));
}
//*判断点在凸多边形内
//点形成一个凸包,而且按逆时针排序(如果是顺时针把里面的<0改为>0)
//点的编号:0~n-1
//返回值:
//-1:点在凸多边形外
//0:点在凸多边形边界上
//1:点在凸多边形内
int inConvexPoly(Point a,Point p[],int n) {
    for(int i = 0; i < n; i++) {
        if(sgn((p[i]-a)^(p[(i+1)%n]-a)) < 0)return -1;
        else if(OnSeg(a,Line(p[i],p[(i+1)%n])))return 0;
    }
    return 1;
}
//*判断点在任意多边形内
//射线法,poly[]的顶点数要大于等于3,点的编号0~n-1
//返回值
//-1:点在凸多边形外
//0:点在凸多边形边界上
//1:点在凸多边形内
int inPoly(Point p,Point poly[],int n) {
    int cnt;
    Line ray,side;
    cnt = 0;
    ray.s = p;
    ray.e.y = p.y;
    ray.e.x = -100000000000.0;//-INF,注意取值防止越界
    for(int i = 0; i < n; i++) {
        side.s = poly[i];
        side.e = poly[(i+1)%n];
        if(OnSeg(p,side))return 0;
//如果平行轴则不考虑
        if(sgn(side.s.y - side.e.y) == 0)
            continue;
        if(OnSeg(side.s,ray)) {
            if(sgn(side.s.y - side.e.y) > 0)cnt++;
        } else if(OnSeg(side.e,ray)) {
            if(sgn(side.e.y - side.s.y) > 0)cnt++;
        } else if(inter(ray,side))
            cnt++;
    }
    if(cnt % 2 == 1)return 1;
    else return -1;
}
//判断凸多边形
//允许共线边
//点可以是顺时针给出也可以是逆时针给出
//点的编号1~n-1
bool isconvex(Point poly[],int n) {
    bool s[3];
    memset(s,false,sizeof(s));
    for(int i = 0; i < n; i++) {
        s[sgn( (poly[(i+1)%n]-poly[i])^(poly[(i+2)%n]-poly[i]) )+1] = true;
        if(s[0] && s[2])return false;
    }
    return true;
}
/*
* 求凸包,Graham算法
* 点的编号0~n-1
* 返回凸包结果Stack[0~top-1]为凸包的编号
*/
const int MAXN = 1010;
Point list[MAXN];
int Stack[MAXN],top;
//相对于list[0]的极角排序
bool _cmp(Point p1,Point p2) {
    double tmp = (p1-list[0])^(p2-list[0]);
    if(sgn(tmp) > 0)return true;
    else if(sgn(tmp) == 0 && sgn(dist(p1,list[0]) - dist(p2,list[0])) <= 0)
        return true;
    else return false;
}
void Graham(int n) {
    Point p0;
    int k = 0;
    p0 = list[0];
//找最下边的一个点
    for(int i = 1; i < n; i++) {
        if( (p0.y > list[i].y) || (p0.y == list[i].y && p0.x > list[i].x) ) {
            p0 = list[i];
            k = i;
        }
    }
    swap(list[k],list[0]);
    sort(list+1,list+n,_cmp);
    if(n == 1) {
        top = 1;
        Stack[0] = 0;
        return;
    }
    if(n == 2) {
        top = 2;
        Stack[0] = 0;
        Stack[1] = 1;
        return ;
    }
    Stack[0] = 0;
    Stack[1] = 1;
    top = 2;
    for(int i = 2; i < n; i++) {
        while(top > 1 &&
                sgn((list[Stack[top-1]]-list[Stack[top-2]])^(list[i]-list[Stack[top-2]])) <=
                0)top--;
        /*
          凸包边上无点：<=    凸包边上有点：<
        */
        Stack[top++] = i;
    }
}
//过三点求圆心坐标
Point waixin(Point a,Point b,Point c) {
    double a1 = b.x - a.x, b1 = b.y - a.y, c1 = (a1*a1 + b1*b1)/2;
    double a2 = c.x - a.x, b2 = c.y - a.y, c2 = (a2*a2 + b2*b2)/2;
    double d = a1*b2 - a2*b1;
    return Point(a.x + (c1*b2 - c2*b1)/d, a.y + (a1*c2 -a2*c1)/d);
}
//两个圆的公共部分面积
double Area_of_overlap(Point c1,double r1,Point c2,double r2) {
    double d = dist(c1,c2);
    if(r1 + r2 < d + eps)return 0;
    if(d < fabs(r1 - r2) + eps) {
        double r = min(r1,r2);
        return PI*r*r;
    }
    double x = (d*d + r1*r1 - r2*r2)/(2*d);
    double t1 = acos(x / r1);
    double t2 = acos((d - x)/r2);
    return r1*r1*t1 + r2*r2*t2 - d*r1*sin(t1);
}


int main() {
#ifndef ONLINE_JUDGE
    freopen("in.txt", "r", stdin);
#endif

    return 0;
}
