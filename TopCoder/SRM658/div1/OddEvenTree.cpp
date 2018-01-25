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

struct OddEvenTree {
    VI getTree (vector<string> x ) {
        int n = x.size();
        vector<VI> s(n,VI(n,0));
        rp(i,n)rp(j,n) s[i][j]=x[i][j]=='O'?1:0;
        VI res;
        rp(i,n) if (s[i][i]) return {-1};
        rp(i,n) rep(j,i,n) {
            if (s[i][j] != s[j][i]) return {-1};
            rp(k,n) {
                if (s[i][k]^s[j][k]^s[i][j])
                    return {-1};
            }
        }
        VI fa(n, -1);
        rp(i,n) rp(j,n) if (s[i][j] && fa[j]==-1) fa[j]=i;
        rep(i,1,n) if (fa[i] == -1) return {-1};
        rep(i,1,n) res.pb(fa[i]), res.pb(i);
        return res;
    }
};
