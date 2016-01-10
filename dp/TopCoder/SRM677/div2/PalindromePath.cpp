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
struct PalindromePath {
    const static int N = 25;
    const int inf = 0x3f3f3f3f;
    int v[N][N];
    vector<char> c[N];
    int shortestLength(int n, vector <int> a, vector <int> b, string s) {
        rp(i, n) c[i].assign(n, '*');
        mst(v, inf);
        rp(i, a.size()) {
            c[a[i]][b[i]] = s[i];
            c[b[i]][a[i]] = s[i];
            v[a[i]][b[i]] = 1;
            v[b[i]][a[i]] = 1;
        }
        rp(i, n) v[i][i] = 0;
        for (int p = 1; p; ) {
            p = 0;
            rp(a, n) rp(b, n) {
                if (v[a][b] == inf) continue;
                rp(i, n) rp(j, n) {
                    if (c[a][i] != '*' && c[a][i] == c[b][j]) {
                        if (v[a][b] + 2 < v[i][j]) {
                            p = 1;
                            v[i][j] = v[a][b] + 2;
                        }
                    }
                }
            }
        }
        return v[0][1]==inf?-1:v[0][1];
    }
};
int main() {
    PalindromePath p;
    assert(p.shortestLength(6, {0,2,3,4,5}, {2,3,4,5,1}, "abaaa") == -1);
    return 0;
}

