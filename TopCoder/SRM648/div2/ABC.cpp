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

struct ABC {
    string createString(int n, int k);
};

const int N = 32;
int dp[2][N][N][N*N];
string res[2][N][N][N*N];

string ABC::createString(int N, int K) {
    int f = 0;
    repd(na, 0, N) repd(nb, 0, N) {
        int nc = N - na - nb;
        if (nc < 0) continue;
        if ( na*(N-na) + nb*(N-na-nb) >= K ) f = 1;
    }
    if (!f) return "";
    mst(dp, 0);
    int cur = 0, nxt = 1;
    dp[cur][1][0][0] = 1;
    res[cur][1][0][0] = "A";
    dp[cur][0][1][0] = 1;
    res[cur][0][1][0] = "B";
    dp[cur][0][0][0] = 1;
    res[cur][0][0][0] = "C";
    repd(n, 1, N-1) {
        mst(dp[nxt], 0);
        repd(na, 0, n) repd(nb, 0, n) {
            int nc = n - na - nb;
            if (nc < 0) continue;
            int mk = na*(n-na) + nb*(n-na-nb);
            repd(k, 0, mk) if (dp[cur][na][nb][k]) {
                string & s = res[cur][na][nb][k];
                dp[nxt][na+1][nb][k] = 1; res[nxt][na+1][nb][k]=s+'A';
                dp[nxt][na][nb+1][k+na] = 1; res[nxt][na][nb+1][k+na]=s+'B';
                dp[nxt][na][nb][k+na+nb] = 1; res[nxt][na][nb][k+na+nb]=s+'C';
            }
        }
        cur = nxt;
        nxt = 1-nxt;
    }
    repd(i, 0, N) repd(j, 0, N) if (dp[cur][i][j][K]) {
        return res[cur][i][j][K];
    }
    return "";
}

int main() {
    ABC a;
    cout<<a.createString(
            30,345
        )<<endl;
    return 0;
}
