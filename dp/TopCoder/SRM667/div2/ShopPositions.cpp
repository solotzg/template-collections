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
struct ShopPositions {
    const static int N = 35;
    int dp[N][N][N], n, m;
    VI c;
    int f(int vdx_2, int vdx_1, int dx) {
        if (dx == n) {
            if(vdx_1) return c[(dx-1)*3*m+vdx_1+vdx_2-1]*vdx_1;
            return 0;
        }
        int & res = dp[vdx_2][vdx_1][dx];
        if (res != -1) return res;
        res = 0;
        repd(v,0,m) {
            int tmp = 0;
            if (vdx_1) tmp += c[(dx-1)*3*m+vdx_1+vdx_2+v-1]*vdx_1;
            tmp += f(vdx_1, v, dx+1);
            res = max(tmp, res);
        }
        return res;
    }
    int maxProfit(int n, int m, VI c) {
        this->n = n;
        this->m = m;
        this->c = move(c);
        mst(dp, -1);
        int res = 0;
        if (n == 1) {
            repd(a,1,m) res = max(res, this->c[a-1]*a);
            return res;
        }
        repd(a,0,m) repd(b,0,m) {
            int tmp = 0;
            if (a) tmp += this->c[a+b-1]*a;
            tmp += f(a,b,2);
            res = max(tmp, res);
        }
        return res;
    }
};
int main() {
    auto a = new ShopPositions;
    assert(a->maxProfit(
               1,5,{100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 1, 1, 1, 1, 1}
                           ) == 300);
    return 0;
}

