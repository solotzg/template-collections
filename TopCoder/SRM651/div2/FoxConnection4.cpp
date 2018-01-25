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
struct FoxConnection4{
    int howManyWays(vector <string> board, int k);
};

const int N = 20;
ll x[N], y[N];
int res;
int dx[] = {0, 0, 1,-1};
int dy[] = {1,-1, 0, 0};
int xx[N], yy[N];
char mmp[N][N];
int r,c, pp[N], k;
set<pair<ll,ll> > sset;
inline int check(int x, int y) {
    return x>=0 && x < r && y >= 0 && y < c && mmp[x][y]!='#';
}
pair<ll,ll> m_hash(int ex[], int ey[]) {
    ll r = 0, t = 0;
    rp(i, k) r = r*20 + (ex[i]+10);
    rp(i, k) t = t*20 + (ey[i]+10);
    return mp(r,t);
}
void solve() {
    static int cpx[N], cpy[N];
    memcpy(cpx, xx, sizeof xx);
    memcpy(cpy, yy, sizeof yy);
    // sort cpx, cpy
    int n = k;
    rp(i, n) rep(j, i+1, n)
        if (cpx[i]>cpx[j] || (cpx[i]==cpx[j]&& cpy[i]>cpy[j]))
            swap(cpx[i], cpx[j]), swap(cpy[i], cpy[j]);
    urp(i, n-1, 0) cpx[i] -= cpx[0], cpy[i]-=cpy[0];
    pair<ll,ll> vhash = m_hash(cpx, cpy);
    if (sset.find(vhash) != sset.end()) return;
    sset.insert(vhash);
    int f = 1;
    rp(i,r)rp(j,c) if (mmp[i][j] == '.') {
        f = 1;
        rp(o, k) if (!check(i+cpx[o], j+cpy[o])) {
            f = 0;
            break;
        }
        res += f;
    }
}
void dfs(int idx) {
    if (idx == k) {
        solve();
        return;
    }
    int nx, ny, f;
    rp(i, idx) rp(j, 4) {
        nx = xx[i]+dx[j];
        ny = yy[i]+dy[j];
        f = 1;
        rp(o, idx) if (xx[o]==nx && yy[o] == ny) {f = 0;break;}
        if (!f) continue;
        xx[idx] = nx;
        yy[idx] = ny;
        dfs(idx+1);
    }
}
int FoxConnection4::howManyWays(vector <string> b, int _k){
    r = b.size();
    c = b[0].size();
    k = _k;
    rp(i, r) rp(j, c) mmp[i][j] = b[i][j];
    res = 0;
    xx[0] = yy[0] = 0;
    dfs(1);
    return res;
}
