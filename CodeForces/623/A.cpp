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
const int N = 505;
int mmp[N][N];
int res[N];
int n, m;
int cal() {
    rp(i, n) {
        rp(j, n) if(mmp[i][j] == 0) {
            if (res[i] == -1 && res[j] == -1) res[i] = 0, res[j]=2;
            else if (res[j] == -1) res[j] = 2-res[i];
            else if (res[i] == -1) res[i] = 2-res[j];
            else if (res[j]+res[i] != 2)
                return 0;
        }
    }
    rp(i, n) if (res[i] == -1) res[i] = 1;
    rp(i,n) rp(j,n) {
        if (mmp[i][j]) {
            if (abs(res[i]-res[j]) > 1) return 0;
        }else if (abs(res[i]-res[j]) != 2) return 0;
    }
    return 1;
}
int main() {
    //freopen("in.txt", "r", stdin);

    //
    mst(mmp, 0);
    mst(res, -1);
    scanf("%d%d", &n, &m);
    rp(i,n) mmp[i][i]=1;
    rp(j,m) {
        int u, v;
        scanf("%d%d", &u, &v);
        --u, --v;
        mmp[u][v]=mmp[v][u] = 1;
    }
    if (!cal()) puts("No");
    else {
        puts("Yes");
        rp(i,n) printf("%c", 'a'+res[i]);
        puts("");
    }
    return 0;
}
