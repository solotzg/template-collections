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

struct ParenthesesDiv2Hard {
    int minSwaps(string s, vector <int> L, vector <int> R) {
        int n = s.size();
        int a = 0, b = 0;
        rp(i, n) {
            if (s[i]=='(') a += 1;
            else b += 1;
        }
        int m = L.size();
        VI stk(n);
        VI cnt(m), sn(m);
        int top;
        int res = 0;
        rp(i, m) {
            int l = L[i], r = R[i];
            if ((r-l+1) & 1)
                return -1;
            top = 0;
            repd(j, l, r) {
                if (s[j] == '(') stk[top++] = j;
                else {
                    if (top == 0 || s[ stk[top-1] ] == ')') stk[top++] = j;
                    else --top;
                }
                if (s[j]=='(') a -= 1;
                else b -= 1;
            }
            int p = 0;
            sn[i] = s[stk[0]]==')';
            rp(j, top) {
                ++p;
                if (j && s[stk[j]] != s[stk[j-1]]) {
                    int la = j, lb = top-la;
                    p = max(la, lb) - min(la, lb);
                    if (la > lb) sn[i] = 1;
                    else sn[i] = 0;
                    res += (min(la, lb)+1)/2;
                    break;
                }
            }
            cnt[i] = p;
        }
        int ca = 0, cb = 0;
        rp(i, m) {
            if (sn[i] == 1) cb += cnt[i];
            else ca += cnt[i];
        }
        if (ca > cb) {
            res += cb/2;
            ca -= cb;
            if (ca / 2 > b) return -1;
            res += ca/2;
        } else {
            res += ca/2;
            cb -= ca;
            if (cb /2 > a) return -1;
            res += cb/2;
        }
        return res;
    }
};
int main() {
#ifdef _TZG_DEBUG
    freopen("in.txt", "r", stdin);
#else
    freopen("C-small-attempt1.in", "r", stdin);
    freopen("out_c.txt", "w", stdout);
#endif
    ParenthesesDiv2Hard p;
    auto v  = p.minSwaps(
                  "()(())))())(())()((())))((()))))(()(((()((", {6, 30, 18, 0}, {17, 35, 21, 5}
              );
    cout<<v<<endl;
    return 0;
}

