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
int dir[4][2] = {1,0,-1,0,0,1,0,-1};
struct BoardEscape {
    int check[5];
    int nim[100][55][55];
    int r , c, cl, ck;
    int cmp(int k, int o) {
        rp(i,r) rp(j,c)
            if (nim[k][i][j] != nim[o][i][j])
                return 0;
        return 1;
    }
    void init(vector <string> &s, int kk) {
        r = s.size(), c = s[0].size();
        cl = -1;
        mst(nim[0], 0);
        repd(k,1,kk) {
            rp(i,r) rp(j,c) {
                char p = s[i][j];
                if (p == 'E')
                    nim[k][i][j] = 0;
                else if (p == '#') nim[k][i][j] = 0;
                else {
                    mst(check, 0);
                    rp(d,4) {
                        int x = i+dir[d][0], y = j+dir[d][1];
                        if (x >= 0 && x < r && y >= 0 && y < c && s[x][y] != '#') {
                            if (nim[k-1][x][y] < 5)
                                check[nim[k-1][x][y]] = 1;
                        }
                    }
                    rp(o,5) {
                        if (check[o] == 0) {
                            nim[k][i][j] = o;
                            break;
                        }
                    }
                }
            }
            rep(o,0,k) {
                if (cmp(k, o)) {
                    cl = o;
                    ck = k;
                    return;
                }
            }
        }
    }
    string findWinner(vector <string> s, int kk) {
        init(s, kk);
        int ok;
        int res = 0;
        if (cl != -1) ok = (kk-cl)%(ck-cl)+cl;
        else ok = kk;
        rp(i,r)rp(j,c) if (s[i][j] == 'T')
            res ^= nim[ok][i][j];
        if (res) return ("Alice");
        else return ("Bob");
    }
};
int main() {
#ifndef ONLINE_JUDGE
    freopen("in.txt", "r", stdin);
#endif
    BoardEscape *b = new BoardEscape;
    cout<<b->findWinner({"TE"},2)<<endl;
    return 0;
}
