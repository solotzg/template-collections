#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
typedef long long ll;
struct BearPaints{
    long long maxArea(int _W, int _H, long long M) {
        ll W = _W;
        ll H = _H;
        if (W*H <= M) return W*H;
        if (W - M / H > H - M/W)
            swap(H, W);
        ll res = -1;
        for (ll a = max(M/H, 1ll), b; a <= W; ++a) {
            b = min(M/a, H) * a;
            if (b > res)
                res = b;
        }
        return res;
    }
};
