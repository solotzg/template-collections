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

const int N = 55;

struct Decipherability {
    string check(string s, int K);
};

string s;
int o;
int dp[N][N][N][2];
int cal() {
    int n = s.size();
    o = n-o;
    if (o == n) return 1;
    mst(dp, 0);
    repd(i,0,n) repd(j,0,n) dp[i][j][0][1] = 1;
    rp(lena, n) {
        rp(lenb, n) {
            repd(k, 1, o) {
                int * p = dp[lena+1][lenb+1][k];
                if (s[lena] == s[lenb]) {
                    if (lena == lenb) {
                        p[1] |= dp[lena][lenb][k-1][1];
                    }
                    ///
                    if (lena != lenb) {
                        p[0] |= dp[lena][lenb][k-1][1];
                    }
                    p[0] |= dp[lena][lenb][k-1][0];

                }
                p[1] |= dp[lena][lenb+1][k][1];
                p[1] |= dp[lena+1][lenb][k][1];
                p[0] |= dp[lena][lenb+1][k][0];
                p[0] |= dp[lena+1][lenb][k][0];
                //printf("%d %d %d %d %d\n", lena+1, 1+lenb, k, p[0], p[1]);
            }
        }
    }
    return !dp[n][n][o][0];
}
string Decipherability::check(string _s, int K) {
    s = _s;
    o = K;
    if (cal()) return "Certain";
    return "Uncertain";
}
int main() {
    Decipherability d;
    cout<<d.check(
                  "wolfsothe",8
                  )<<endl;
    return 0;
}
