#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
#define vtr(v) (v).begin(),(v).end()
#define mp(a,b) make_pair(a,b)
#define fi first
#define se second
#define pb(a) push_back(a)
#define _0(x) (!(x))
#define _1(x) (x)
#define bit(x,y) (((x)>>(y))&1)
typedef vector<int> VI;
typedef pair<int,int> PII;
typedef vector<PII> VPII;
typedef long long ll;

const double eps = 1e-8;
const double PI = acos(-1.0);
inline int sgn(double x) {
    if(fabs(x) < eps)return 0;
    if(x < 0)return -1;
    else return 1;
}
struct Point {
    double x,y;
    int id;
    Point() {}
    Point(double _x, double _y, int _id = -1) {
        x = _x;
        y = _y;
        id = _id;
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
//*两点间距离
double dist(const Point & a, const Point & b) {
    return sqrt((a-b)*(a-b));
}
const int MAXN = 100;
Point lst[MAXN], cp[MAXN], base;
int Stack[MAXN],top;
//相对于lst[0]的极角排序
bool _cmp(const Point & p1,const Point & p2) {
    double tmp = (p1-base)^(p2-base);
    if(sgn(tmp) > 0)return true;
    else if(sgn(tmp) == 0 && sgn(dist(p1,base) - dist(p2,base)) <= 0)
        return true;
    else return false;
}
int p = 0;
void Graham(int n) {
    if (!p) {
        Point p0;
        int k = 0;
        p0 = lst[0];
        for(int i = 1; i < n; i++) {
            if( (p0.y > lst[i].y) || (p0.y == lst[i].y && p0.x > lst[i].x) ) {
                p0 = lst[i];
                k = i;
            }
        }
        swap(lst[k],lst[0]);
    }
    base = lst[0];
    sort(lst+1,lst+n,_cmp);
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
                sgn((lst[Stack[top-1]]-lst[Stack[top-2]])^(lst[i]-lst[Stack[top-2]])) <=
                0)top--;
        Stack[top++] = i;
    }
}
struct NoRightTurnDiv2 {
    int n;
    int vis[MAXN];
    VI findPath(VI x, VI y) {
        n = x.size();
        rp(i, n) lst[i] = Point(x[i], y[i], i);
        VI res;
        while (n) {
            Graham(n);
            memcpy(cp, lst, sizeof lst);
            mst(vis, 0);
            rp(i, top) {
                res.pb(cp[Stack[i]].id);
                vis[Stack[i]] = 1;
            }
            int k = 0;
            rp(i, n) {
                if (!vis[i]) lst[k++] = cp[i];
            }
            n -= top;
            p = 1;
            ///
            base = cp[Stack[top-1]];
            sort(lst, lst+n, _cmp);
        }
        return res;
    }
};
