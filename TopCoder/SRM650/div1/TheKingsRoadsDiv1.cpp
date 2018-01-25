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

struct TheKingsRoadsDiv1 {
    string getAnswer(int h, vector <int> a, vector <int> b);
};
const int N = 1030;
const string yes = "Correct", no = "Incorrect";
int n, h;
VI e[N];
int q[N*N], dep[N];
int cal(int l, int r) {
    if (l == n) return 1;
    if (l >= r) return 0;
    int pos = q[l];
    if (dep[pos] == h-1) return cal(l+1, r);
    int nxt[6], len = 0;
    for (int v : e[pos]) if (dep[v] == -1) {
            nxt[len++] = v;
            if (len > 5) return 0;
        }
    sort(nxt, nxt+len);
    len = unique(nxt, nxt+len) - nxt;
    if (len < 2) return 0;
    rp(i, len) rep(j, i+1, len) {
        dep[nxt[i]] = dep[pos] + 1;
        dep[nxt[j]] = dep[pos] + 1;
        q[r] = nxt[i];
        q[r+1] = nxt[j];
        if (cal(l+1, r+2)) return 1;
        dep[nxt[i]] = -1;
        dep[nxt[j]] = -1;
    }
    return 0;
}
string TheKingsRoadsDiv1::getAnswer(int _h, vector <int> a, vector <int> b) {
    h = _h;
    n = (1<<h)-1;
    rp(i, a.size()) {
        int u = a[i]-1, v = b[i]-1;
        e[u].pb(v);
        e[v].pb(u);
    }
    rp(root, n) {
        mst(dep, -1);
        dep[root] = 0;
        q[0] = root;
        if (cal(0, 1) )
            return yes;
    }
    return no;
}

int main() {
    TheKingsRoadsDiv1 t;
    cout<<t.getAnswer(
            3,
    {1, 3, 2, 2, 3, 7, 1, 5, 4},
    {6, 5, 4, 7, 4, 3, 3, 1, 7}
        )<<endl;
    return 0;
}
