#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
#define vtr(v) (v).begin(),(v).end()
typedef long long ll;
typedef vector<int> VI;
struct FindingKids{
    const int M = 1e9+7;
    VI lps, rps, pos;
    ll res;
    int t;
    int get_cnt(int x) {
        return lower_bound()
    }
    ll getSum(int n, int q, int a, int b, int c){
        int p, A = a, B = b, C = c;
        set<int> vis;
        rp(i,n) {
            A = ((ll)A*B+C)%M;
            p = A%(M-n+i+1);
            if (vis.find(p) != vis.end())
                p = M-n+i;
            if (p&1)
                lps.push_back(p);
            else
                rps.push_back(p);
            vis.insert(p);
            pos.push_back(p);
        }
        VI spos(vtr(pos));
        sort(vtr(lps));
        sort(vtr(rps));
        sort(vtr(spos));
        A = a, B = b, C = c;
        res = 0ll;
        rp(i,q) {
            A = ((ll)A*B+C)%M;
            int kid = A%n;
            A = ((ll)A*B+C)%M;
            t = A;
            int p = lower_bound(vtr(spos), pos[kid])-spos.begin()+1;
            int bg = -M, ed = 2*M+1, half;
            ll len = (ll)ed-bg;
            while (len > 0) {
                half = len>>1;
                int mid = half + bg;
                if (get_cnt(mid) < p) {
                    bg = mid+1;
                    len = len-half+1;
                }
                else
                    len = half;
            }
            res += bg;
        }
        return res;
    }
};
int main() {

    return 0;
}
