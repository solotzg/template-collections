#include <cstdio>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstring>
#include <queue>
#include <set>
#include <cmath>
#include <map>
#include <queue>
#include <stack>
#include <sstream>
using namespace std;
typedef long long ll;
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b); i>=__tzg_##i; --i)
#define rp(i,b) for(int i=(0), __tzg_##i=(b);i<__tzg_##i;++i)
#define rep(i,a,b) for(int i=(a), __tzg_##i=(b);i<__tzg_##i;++i)
#define repd(i,a,b) for(int i=(a), __tzg_##i=(b);i<=__tzg_##i;++i)
#define mst(a,b) memset(a,b,sizeof(a))
#define px first
#define py second
#define __0(x) (!(x))
#define __1(x) (x)
typedef pair<int,int> pii;
const ll mod = 1000000007;
const int MAXM = 15;
const double eps = 1e-6;
const int inf = 0x3f3f3f3f;
#define mp(a,b) make_pair(a,b)
typedef vector<int> VI;
typedef vector<ll> VL;
typedef vector<pii> VPII;
typedef vector<string> VS;
typedef vector<VI> VVI;
typedef vector<VL> VVL;
int gcd(int a, int b) {
    while (b) {
        int t = b;
        b = a%b;
        a = t;
    }
    return a;
}
inline pii calc(int a, int b) {
    int g = gcd(a, b);
    return mp(a/g, b/g);
}
struct BearDarts {
    typedef map<pii, VI> T;
    ll count(VI w) {
        int n = w.size();
        T mmp;
        T::iterator it;
        rp(i,n) rep(j, 0, i) {
            mmp[calc(w[j], w[i])].push_back(i);
        }
        ll res = 0;
        rp(i, n) rp(j, i) {
            pii p = calc(w[i], w[j]);
            if ((it=mmp.find(p)) != mmp.end()) {
                VI & v = it->py;
                int o = lower_bound(v.begin(), v.end(), j) - v.begin();
                res += o;
            }
        }
        return res;
    }
};
