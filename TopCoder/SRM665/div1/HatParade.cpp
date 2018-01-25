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
typedef pair<int, int> PII;
typedef vector<PII> VPII;
typedef long long ll;
struct HatParade {
    int getPermutation(vector <int> v, vector <int> s) {
        const ll mod = 1e9 + 7;
        int n = v.size();
        ll C = 0;
        rp(i, n) {
            C += v[i];
        }
        vector<pair<ll, int > > list;
        vector<int> e;
        rp(i, n) {
            ll a = min((ll)s[i] - v[i], C - s[i]);
            list.pb(mp(a, i));
            e.pb(a);
        }
        sort(list.begin(), list.end());
        ll sl = 0, sr = 0, res = 1ll;
        rp(i, n) {
            pair<ll, int> p = list[i];
            if(sr != e[p.se]) {
                return 0;
            }
            if(sl == sr && i != n - 1)
                res = res * 2ll % mod;
            sr += v[p.se];
            if(sl < sr)
                swap(sl, sr);
        }

        return res;
    }
};
int main() {
    HatParade h;assert(h.getPermutation({1,1,1}, {1,1,2}) == 2);
    return 0;
}
