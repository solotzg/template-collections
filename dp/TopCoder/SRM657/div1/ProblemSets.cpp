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

inline ll min(ll a, ll b, ll c) {
    return min(min(a, b), c);
}
class ProblemSets {
public:
    ll e, em, m, mh, h;
    int check(ll ta) {
        if (ta > e+em) return 0;
        if (ta > e) em -= ta-e;
        if (ta > em+m+mh) return 0;
        if (ta > em+m) mh -= ta-em-m;
        if (ta > mh + h) return 0;
        return 1;
    }
    ll maxSets(ll E, ll EM, ll M, ll MH, ll H) {
        ll bg = 1, ed = ((~0ull)>>1);

        while (bg < ed) {
            ll md = (bg+ed+1)>>1;
            e = E;
            em = EM;
            m = M;
            mh = MH;
            h = H;
            if (check(md)) bg = md;
            else ed = md-1;
        }
        return bg;
    }
};

int main() {
    return 0;
}

