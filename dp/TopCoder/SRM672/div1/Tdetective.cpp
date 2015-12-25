#include <bits/stdc++.h>
using namespace std;
#define rep(i,a,b) for(int i=(a),__tzg_##i=(b);i<__tzg_##i;++i)
#define urp(i,a,b) for(int i=(a),__tzg_##i=(b);i>=__tzg_##i;--i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,(b)+1)
#define mst(a,b) memset(a,b,sizeof(a))
#define vrp(it,v) for(auto it(v.begin());(it)!=(v.end());++it)
typedef long long ll;
struct Tdetective {
    const static int N = 55, inf = 0x3f3f3f3f;
    int f[N], p[N][N], r[N][N], o[N];
    int find(int x) {
        return f[x]=f[x]==x?x:find(f[x]);
    }
    int reveal(vector <string> s) {
        int n = s.size(), ans = 0;
        rp(i,n)rp(j,n) p[i][j]=s[i][j]-'0';
        rp(i,n)rp(j,n) r[i][j]=i==j?0:inf;
        rp(i,n) f[i]=i,o[i]=1;
        urp(i,9,0) {
            rp(a,n)rp(b,n)if(p[a][b]==i&&find(a)!=find(b))
                rp(c,n)if(find(a)==find(c))
                    r[c][b] = min(r[c][b], o[find(c)]);
            rp(a,n)rp(b,n)if(p[a][b]==i&&find(a)!=find(b))
                o[find(a)] += o[find(b)], f[find(b)] = find(a);
        }
        rp(k,n)rp(i,n)rp(j,n)if(i!=j&&j!=k&&i!=k) r[i][j]=min(r[i][j], r[i][k]+r[k][j]);
        rp(i,n) ans += i*r[0][i];
        return ans;
    }
};
int main() {
    Tdetective *t = new Tdetective;
    cout<<t->reveal({"0886","8086","8801","6610"})<<endl;
    return 0;
}
