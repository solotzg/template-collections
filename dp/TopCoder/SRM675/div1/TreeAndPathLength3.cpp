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
typedef vector<int> VI;
typedef pair<int, int> PII;
typedef vector<PII> VPII;
typedef long long ll;
struct TreeAndPathLength3{
    VI cal(int a, int b, int c) {
        int s1 = 0, s2 = 1, cnt = 2;
        VI r;
        rp(i,a) {
            r.pb(s1);
            r.pb(cnt++);
        }
        r.pb(s1);
        r.pb(s2);
        rp(i,b) {
            r.pb(s2);
            r.pb(cnt++);
        }
        rp(i,c) {
            r.pb(cnt-1);
            r.pb(cnt++);
        }
        return r;
    }
    vector <int> construct(int s){
        repd(a,1,500) {
            repd(b,1,500) {
                if(a+b+2 > 500) break;
                repd(c,0,500) {
                    if(a+b+c+2 > 500) break;
                    if (a*b+b-1+c == s) {
                        return cal(a,b,c);
                    }
                }
            }
        }
        return {};
    }
};
