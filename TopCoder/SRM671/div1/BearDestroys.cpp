#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
typedef long long ll;
struct BearDestroys{
    int MOD, W, H;
    int dp[44][14][1<<13][2];
    int pow2[14*35];
    int add(int a, int b) {
        a += b;
        return a>=MOD?a-MOD:a;
    }
    int calc(int d, int y, int mask, int last, int filled) {
        ll res = 0;
        if (d == W+H-1)
            return 0;
        else if (y >= H || (d-y) < 0 ) {
            return calc(d+1, 0, last?(mask|(1<<y)):(mask), 0, filled);
        } else if (d-y >= W) {
            return calc(d, y+1, mask, 0, filled);
        } else {
            if (dp[d][y][mask][last] != -1)
                return dp[d][y][mask][last];
            int umask = (~(1<<y))&mask;
            int smask = mask|(1<<y);
            int R = -1, D = -1, O = -1;
            int down = 1, right = 1;
            int x = d-y;
            if ((1<<y)&mask || y == H-1) down = 0;
            if ((1<<y)&mask || x == W-1 || last ) right = 0;
            O = calc(d, y+1, last?smask:umask, 0, 1+filled);
            if (right)
                R = add(pow2[W*H-filled-1], calc(d, y+1, smask, 0, 1+filled));
            if (down)
                D = add(pow2[W*H-filled-1], calc(d, y+1, last?smask:umask, 1, 1+filled));

            if (right) res = add(res, R);
            else if (down) res = add(res, D);
            else res = add(res, O);

            if (down) res = add(res, D);
            else if (right) res = add(res, R);
            else res = add(res, O);
        }
        return dp[d][y][mask][last] = res;
    }
    int sumUp(int _W, int _H, int _MOD) {
        MOD = _MOD;
        W = _W;
        H = _H;
        mst(dp, -1);
        pow2[0] = 1;
        repd(i,1,W*H)
            pow2[i] = (ll) pow2[i-1]*2%MOD;
        return calc(0,0,0,0,0);
    }
};
int main() {
    unique_ptr<BearDestroys> p(new BearDestroys);
    cout<<p->sumUp(	30, 13, 748527467)<<endl;
    return 0;
}
