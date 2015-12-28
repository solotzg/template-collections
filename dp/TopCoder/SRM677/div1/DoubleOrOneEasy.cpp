#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
typedef long long ll;
typedef vector<int> VI;
vector<int> cal(int x) {
    VI r, k;
    while (x) {
        if (x & 1)
            r.push_back(1);
        else
            r.push_back(0);
        x >>= 1;
    }
    urp(i,r.size()-1, 0) k.push_back(r[i]);
    return k;
}
int fuck(VI & a, int n) {
    int r = 0;
    rp(i,n) {
        r = r*2 + a[i];
    }
    return r;
}
struct DoubleOrOneEasy{
    const int INF = 0x3f3f3f3f;
    int minimalSteps(int a, int b, int newA, int newB) {
        VI va = cal(a), vb = cal(b), vna = cal(newA), vnb = cal(newB);
        int la = va.size(), lb = vb.size(), lna = vna.size(), lnb = vnb.size();
        if (newA < a || newB < b) return -1;
        repd(i,la,lna) {
            int u = fuck(vna, i);
            if (u < a) continue;
            int o = u-a;
            int bb = b+o;
            int p = lnb - (lna-i);
            if (p <= 0) continue;
            int lnbb = fuck(vnb, p);
            if (bb != lnbb)
                continue;
            int f = 1, c = 0;
            rp(j,lna-i) {
                if (vna[lna-1-j] != vnb[lnb-1-j]) {
                    f = 0;
                    break;
                } else {
                    c += vna[lna-1-j];
                }
            }
            if (!f) continue;
            return o + c + lna-i;
        }
        return -1;
    }
};
int main() {
    DoubleOrOneEasy d;
    cout<<d.minimalSteps(1,1,1,1)<<endl;
    return 0;
}
