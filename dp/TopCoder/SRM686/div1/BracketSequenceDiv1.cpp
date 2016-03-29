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
#define bit(x,y) (((x)>>(y))&1) 
typedef vector<int> VI; 
typedef pair<int,int> PII; 
typedef vector<PII> VPII; 
typedef long long ll; 

const int N = 105; 
map<PII, ll> mmp; 
string s; 
ll dp(int l, int r) { 
    if (l >= r) return 1; 
    auto it = mmp.find(mp(l,r)); 
    if (it != mmp.end()) return it->se; 
    ll res = 1ll; 
    repd(i, l, r) { 
        char c = s[i]; 
        { 
            switch (c) { 
            case '(': 
                repd(j, i+1, r) if (s[j] == ')') 
                    res += dp(i+1, j-1)*dp(j+1,r); 
                break; 
            case '[': 
                repd(j, i+1, r) if (s[j] == ']') 
                    res += dp(i+1, j-1)*dp(j+1,r); 
                break; 
            } 
        } 
    } 
    return mmp[mp(l,r)] = res; 
} 
struct BracketSequenceDiv1 { 
    long long count(string _s) { 
        s = _s; 
        return dp(0, s.size()-1) - 1ll; 
    } 
};
