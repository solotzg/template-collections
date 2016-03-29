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

struct Point {
    int x,y;
    Point() {}
    Point(int _x, int _y) {
        x = _x;
        y = _y;
    }
    Point operator - (const Point &b)const {
        return Point(x - b.x,y - b.y);
    }
//²æ»ý
    double operator ^ (const Point &b)const {
        return x*b.y - y*b.x;
    }
//µã»ý
    int operator * (const Point &b)const {
        return x*b.x + y*b.y;
    }
};
inline int cross(const Point & o, const Point & p1, const Point & p2) {
    return (p1-o)^(p2-o);
}
struct TrianglesContainOrigin{
	long long count(vector <int> x, vector <int> y){
		vector<Point> da(x.size());
		int n = x.size();
		rp(i, n) da[i] = {x[i], y[i]};
		ll res = (ll)n * (n-1) * (n-2) / 3ll / 2ll ;
		rp(i, n) {
			///
			int num = 0;
			rp(j, n) if (i != j) {
				int tp = cross(da[i], da[j], Point(0,0) );
				if (tp > 0) ++num;
			}
			///
			ll sum = num*(num-1)/2;
			res -= sum;
		}
		return res;
	}
};
