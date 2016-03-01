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

struct SubtreesCounting {
    const static int N = 1e5+10, MOD = 1e9+7;
    VI e[N];
    inline int add(int a, int b) {
        a += b;
        return a>=MOD?a-MOD:a;
    }
    inline int mul(int a, int b) {
        return (ll)a*b%MOD;
    }
    int ways[N][2], num[N][2];
    void go(int u, int pre) {
        int *nm = num[u], *w = ways[u];
        nm[0] = 0;
        nm[1] = 1;
        for (int v : e[u] ) if (v != pre) {
                go(v, u);
            }
        w[0] = 0;
        w[1] = 1;
        for (int v : e[u] ) if (v != pre) {
                w[0] = add(w[0], ways[v][0]);
                w[1] = add(mul(w[1], 1+num[v][1]), mul(ways[v][1], nm[1]));
                nm[1] = mul(nm[1], num[v][1]+1);
                nm[0] = add(nm[0], num[v][0]);
            }
        nm[0] = add(nm[0], nm[1]);
        w[0] = add(w[0], w[1]);
    }
    int sumOfSizes(int n, int a0, int b, int c, int m) {
        static int a[N];
        a[0] = a0;
        rep(i,1,n-1) a[i] = ((ll)b*a[i-1]+c)%m;
        rep(i,1,n) {
            int j = a[i-1]%i;
            e[i].pb(j);
            e[j].pb(i);
        }
        go(0, -1);
        return ways[0][0];
    }
};
int main() {
    SubtreesCounting *s = new SubtreesCounting;
    cout<<s->sumOfSizes(
            100000,
            123,
            46645,
            4564579,
            1000000000
        )<<endl;
    return 0;
}
