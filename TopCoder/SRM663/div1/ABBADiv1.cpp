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
struct ABBADiv1 {
    string a;
    int check(string && s) {
        if (s == a) return 1;
        if (s.size() >= a.size()) {
            if (s[0] == 'B') {
                string tp = s;
                reverse(vtr(tp));
                if (check(move(tp.substr(0, tp.size()-1)))) return 1;
            }
            if (s.back() == 'A') {
                if (check(move(s.substr(0,s.size()-1)))) return 1;
            }
        }
        return 0;
    }
    string canObtain (string initial, string target) {
        a = move(initial);
        return check(move(target))?"Possible":"Impossible";
    }
};
int main() {
    auto a = new ABBADiv1;
    assert(a->canObtain("AAABAAABB","BAABAAABAABAABBBAAAAAABBAABBBBBBBABB") == "Impossible");
    return 0;
}
