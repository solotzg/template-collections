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
typedef long long ll;
struct ClassicProblem {
    const int M = 31;
    const static int N = 85;
    typedef vector<int> VI;
    vector<pair<int, int> > nd[35];
    VI divide(int x) {
        int y;
        VI r;
        rp(i, M) {
            if((y = 1 << i) <= x) {
                x -= y;
                r.pb(i);
            }
            else break;
        }
        rep(i, 0, M) {
            if(x & (1 << i))
                r.pb(i);
        }
        return r;
    }
    long long maximalValue(vector <int> cnt, vector <int> w, vector <int> v, int limit) {
        const int maxs = N * N * 2 * 2;
        int n = cnt.size();
        rp(i, n) {
            VI vl = divide(cnt[i]);
            for(auto &it : vl) {
                nd[it].pb(mp(w[i], v[i]));
            }
        }
        int cur = 0, pre = 1;
        vector<ll> dp[2];
        dp[cur].assign(maxs, -1);
        dp[cur][0] = 0;
        urp(i, M - 1, 0) {
            swap(cur, pre);
            dp[cur].assign(maxs, -1);
            int y = (limit >> i) & 1;
            rp(j, maxs) dp[cur][j] = dp[pre][(j - y + 1) / 2];
            for(const auto &it : nd[i]) {
                rp(k, maxs - it.fi) if(dp[cur][k + it.fi] >= 0) {
                    dp[cur][k] = max(dp[cur][k], dp[cur][k + it.fi] + (ll)it.se * (1 << i));
                }
            }
        }
        return *max_element(vtr(dp[cur]));
    }
};
int main() {
    ClassicProblem *f = new ClassicProblem;
    assert(f->maximalValue({17452733, 484, 607117, 44102867, 80917, 9955379, 154783, 79, 757486, 1800008, 31397, 66532052, 5, 8351, 30063, 538787, 66, 993, 28503205, 696813, 93438689, 9474478, 14716355, 93438689, 53851, 75482618, 6856, 93438689, 8033317, 31, 4, 2, 2957, 68, 536547, 240613, 19, 290847, 57242428, 1722, 19160318, 692, 73009, 5, 93438689, 1431, 8982}, {12, 22, 6, 16, 14, 20, 10, 7, 5, 2, 12, 11, 11, 10, 9, 5, 8, 3, 1, 18, 9, 7, 22, 21, 19, 12, 15, 20, 11, 17, 7, 14, 11, 15, 5, 4, 22, 6, 11, 21, 17, 20, 22, 18, 12, 6, 6}, {977866, 774579, 977866, 7, 54, 830, 3, 977866, 977866, 27341, 977866, 290, 9, 445629, 977866, 6, 525083, 1, 977866, 41533, 977866, 977866, 977866, 8, 4203, 977866, 62884, 2, 9, 977866, 5005, 2, 727, 9952, 977866, 977866, 275, 70, 61, 834, 651, 977866, 977866, 977866, 977866, 16488, 977866}, 38504) == 37651752464ll);
    return 0;
}
