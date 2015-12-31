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
struct LimitedMemorySeries1 {
    const static int mod = 1e9+7;
    const static int N = sqrt(mod+10);
    int pnum[N], cnum[N];
    long long getSum(int n, int x0, int a, int b, vector <int> query) {
        ll res = 0, x;
        mst(pnum, 0);
        x = x0;
        rp(i,n) {
            ++pnum[x / N];
            x = ((ll)x*a+b)%mod;
        }
        for(int q : query) {
            ++q;
            int sl = 0, sr = 0, dx = -1;
            rp(i,N) {
                sl = sr;
                sr += pnum[i];
                if (sr >= q) {
                    dx = i;
                    break;
                }
            }
            int rl = dx*N, rr = rl+N-1;
            mst(cnum, 0);
            x = x0;
            rp(i,n) {
                if (x >= rl && x <= rr)
                    ++cnum[x-rl];
                x = ((ll)x*a+b)%mod;
            }
            q -= sl;
            int sm = 0;
            rp(i,N) {
                sm += cnum[i];
                if (sm >= q) {
                    res += i+rl;
                    break;
                }
            }
        }
        return res;
    }
};
int main() {
    LimitedMemorySeries1 *d = new LimitedMemorySeries1;
    cout<<d->getSum(
            5000000, 381, 492983, 8478294, {2900000, 2900001, 2900002, 2900003, 2900004, 2900005, 2900006, 2900007, 2900008, 2900009, 2900010, 2900011, 2900012, 2900013, 2900014, 2900015, 2900016, 2900017, 2900018, 2900019, 2900020, 2900021, 2900022, 2900023, 2900024, 2900025, 2900026, 2900027, 2900028, 2900029, 2900030, 2900031, 2900032, 2900033, 2900034, 2900035, 2900036, 2900037, 2900038, 2900039, 2900040, 2900041, 2900042, 2900043, 2900044, 2900045, 2900046, 2900047, 2900048, 2900049, 2900050, 2900051, 2900052, 2900053, 2900054, 2900055, 2900056, 2900057, 2900058, 2900059, 2900060, 2900061, 2900062, 2900063, 2900064, 2900065, 2900066, 2900067, 2900068, 2900069, 2900070, 2900071, 2900072, 2900073, 2900074, 2900075, 2900076, 2900077, 2900078, 2900079, 2900080, 2900081, 2900082, 2900083, 2900084, 2900085, 2900086, 2900087, 2900088, 2900089, 2900090, 2900091, 2900092, 2900093, 2900094, 2900095, 2900096, 2900097, 2900098, 2900099}
        );
    return 0;
}
