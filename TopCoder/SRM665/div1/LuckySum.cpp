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
struct LuckySum {
    const ll inf = 0x3f3f3f3f3f3f3f3f;
    VI s, a, b;
    ll res, t[20];
    int n;
    void dfs(int p, ll sum) {
        if (p == n) {
            res = min(res, sum);
            return;
        }
        for (int A : {0,4,7}) {
            if (p > 0 && A != 0 && a[p-1] == 0) continue;
            if (p == 0 && A == 0) continue;
            for (int B : {0,4,7}) if(A <= B) {
                if (p > 0 && B != 0 && b[p-1] == 0) continue;
                int c = p==0?0:(a[p-1]+b[p-1])/10;
                int d = (c+A+B)/10, e = (c+A+B)%10;
                if (s[p] != -1 && s[p] != e) continue;
                if (s[p] == -1 && p == n-1 && !e) continue;
                if (p == n-1 && d != 0) continue;
                a[p] = A;
                b[p] = B;
                dfs(p+1, sum + e*t[p]);
            }
        }
    }
    long long construct(string note) {
        n = note.size();
        s.resize(n);
        a = b = s;
        rp(i,n) {
            int j = n-1-i;
            s[i] = note[j]=='?'?-1:note[j]-'0';
        }
        t[0] = 1ll;
        rep(i,1,20) t[i] = t[i-1]*10ll;
        res = inf;
        dfs(0, 0ll);
        return res == inf?-1:res;
    }
};
