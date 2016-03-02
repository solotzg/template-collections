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
struct ChangingChange {
    const static int N = 1004005, MOD = 1e9+7;
    int A[N], RA[N], D;
    int get_comb(int n, int k) {
        return mul(A[n], mul(RA[k], RA[n-k]));
    }
    int m_pow(int a, int b) {
        int r = 1, p = a;
        while(b) {
            if (b & 1) r = mul(r, p);
            p = mul(p,p);
            b >>= 1;
        }
        return r;
    }
    inline int mul(int a, int b) {
        return (ll)a*b%MOD;
    }
    inline int add(int a, int b) {
        return (a+=b)>=MOD?a-MOD:(a<0?a+MOD:a);
    }
    void init() {
        A[0] = RA[0] = 1;
        rep(i,1,N) A[i] = mul(A[i-1], i), RA[i] = m_pow(A[i], MOD-2);
    }
    ChangingChange() {
        init();
    }
    VI countWays(VI ways, VI valueRemoved, VI numRemoved) {
        D = ways.size()-1;
        VI res(valueRemoved.size());
        rp(i, valueRemoved.size()) {
            int r = numRemoved[i], v = valueRemoved[i];
            int k = D / v, tp = 0;
            repd(j,0,k) {
                tp = add(tp, mul(ways[D-j*v], mul(get_comb(j+r-1,j), (j&1)?-1:1)));
            }
            res[i] = tp;
        }
        return res;
    }
};
int main() {
    auto a = new ChangingChange;
    assert(a->countWays(
               {1, 4, 6},
               {1, 1, 1},
               {1, 2, 3}
                        ) == VI({3, 1, 0}));
    assert(a->countWays({1, 1}, {1}, {1}) == VI({0}));
    assert(a->countWays({1, 2, 3, 6, 9, 14}, {1, 3, 5}, {1000000, 4, 2}) == VI({34955525, 2, 12}));
    assert(a->countWays({1, 346711436, 448044553, 169076686, 882250998, 402101102, 834542981, 717759958, 307300034, 362068650, 850807177, 856108597, 570084289, 559616718, 990648426, 920747396, 836122971, 203781517, 518275593, 971410743, 972353856}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {710692, 593660, 553871, 927381, 244928, 88921, 254581, 995685, 956643, 565731, 760456, 942564, 401292, 608982, 405395, 130018, 658530, 760100, 880299, 834696})
            ==
            VI({980542230, 960722897, 537923653, 984590528, 804014587, 983785403, 337487216, 480851112, 669336005, 750481313, 380312141, 910781870, 278302610, 665220810, 418935030, 503735045, 42398387, 885891535, 590613738, 416848652}));
    return 0;
}
