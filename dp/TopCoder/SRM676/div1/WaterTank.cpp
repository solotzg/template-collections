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
typedef vector<int> VI;
typedef pair<int,int> PII;
typedef vector<PII> VPII;
typedef long long ll;
struct WaterTank {
    double minOutputRate(vector <int> t, vector <int> x, int C) {
        double s = 0.0, e = 1e6, c = C;
        const double eps = 1e-7;
        while (e-s > eps) {
            double m = (s+e)/2.0;
            double a = 0.0;
            int f = 1;
            rp(i,t.size()) {
                a += t[i]*(x[i]-m);
                if (a > c+eps) {
                    f = 0;
                    break;
                }
                if (a < 0.0) a = 0.0;
            }
            if (f) e = m;
            else s = m;
        }
        return s;
    }
};
