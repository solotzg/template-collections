#include <algorithm>
#include <vector>
#include <cstring>
using namespace std;
class Aho_Corasick {
public:
    const static int N = 100005;
    const static int B = 26;
    int tire[N][B], ntire, fail[N];
    vector<int> vec[N];
    int mmp[N];
    int root;
    void init() {
        ntire = 0;
        root = new_node();
    }
    int new_node() {
        memset(tire[ntire], -1, sizeof(tire[ntire]));
        fail[ntire] = -1;
        vec[ntire].clear();
        return ntire++;
    }
    void insert(const char * s, int bg, int ed, int e) {
        int cur = root;
        for (int i = bg; i<= ed; ++i) {
            int o = s[i]-'a';
            if (tire[cur][o] == -1)
                tire[cur][o] = new_node();
            cur = tire[cur][o];
        }
        vec[cur].push_back(e);
    }
    void build_fail() {
        static int q[N];
        int head = 0, tail = 0;
        for (int i = 0; i< B; ++i) {
            if (tire[root][i] != -1)
                q[tail++] = tire[root][i], fail[tire[root][i]] = root;
            else
                tire[root][i] = root;
        }
        while (head < tail) {
            int pos = q[head++];
            for (int i = 0; i< B; ++i) {
                int & son = tire[pos][i];
                if (son != -1) {
                    fail[son] = tire[fail[pos]][i];
                    q[tail++] = son;
                    const vector<int> & vv = vec[fail[son]];
                    vec[son].insert(vec[son].end(), vv.begin(), vv.end());
                } else {
                    son = tire[fail[pos]][i];
                }
            }
        }
    }
    int check(const char *s, const char *p, int bs, int es, int bp, int ep, int & res) {
        if (bs > es || (ep-bp)>(es-bs) ) return 0;
        init();
        int flg = 0;
        for (int i = bp; i<= ep; ++i) {
            if (p[i] == '?') continue;
            else {
                flg++;
                int j = i;
                while (j <= ep && p[j] != '?') ++j;
                --j;
                insert(p, i, j, j-bp);
                i = j;
            }
        }
        if (flg == 0) {
            res = ep - bp + bs;
            return 1;
        }
        build_fail();
        int cur = root, pos = -1;
        for (int i = bs; i<= es && pos == -1; ++i) {
            int a = s[i]-'a';
            cur = tire[cur][a];
            mmp[i] = 0;
            for (int j = 0; j< (int) vec[cur].size(); ++j) {
                if (i-vec[cur][j] >= bs) {
                    ++mmp[i-vec[cur][j]];
                    if (mmp[i-vec[cur][j]] == flg) {
                        pos = i-vec[cur][j];
                        break;
                    }
                }
            }
        }
        if (pos == -1)
            return 0;
        res = pos + ep - bp;
        if (res > es)
            return 0;
        return 1;
    }
    int match(const char *s, const char *p, int ls, int lp) {
        int tl = 0;
        for (int i = 0; i< lp; ++i) if (p[i] != '*') ++tl;
        if (tl == lp) {
            if (ls != lp) return 0;
            for (int i = 0; i< ls; ++i) {
                if (s[i] != p[i] && p[i] != '?')
                    return 0;
            }
            return 1;
        }
        if (tl == 0) return 1;
        if (tl > ls) return 0;
        int bp = 0, ep = lp-1;
        int bs = 0, es = ls-1;
        for ( ; bp<=ep && bs <= es; ++bp, ++bs) {
            if (p[bp] == '*') {
                break;
            } else if (p[bp] != s[bs] && p[bp] != '?') return 0;
        }
        for ( ; bp<=ep && bs <= es; --ep, --es) {
            if (p[ep] == '*') {
                break;
            } else if (p[ep] != s[es] && p[ep] != '?') return 0;
        }
        for (int i = bp; i<= ep; ++i) {
            if (p[i] == '*') continue;
            else {
                int j = i, k;
                while (p[j] != '*') ++j;
                --j;
                if (!check(s, p, bs, es, i, j, k))
                    return 0;
                bs = k+1;
                i = j;
            }
        }
        return 1;
    }
    bool isMatch(string s, string p) {
        return match(s.c_str(), p.c_str(), s.size(), p.size());
    }
};
Aho_Corasick a;
char ss[100005], pp[100005];
int main() {
#ifndef ONLINE_JUDGE
    freopen("in.txt", "r", stdin);
#endif // ONLINE_JUDGE
    while (scanf("%s%s", ss, pp) != EOF) {
        if (a.match(ss, pp, strlen(ss), strlen(pp)))
            puts("YES");
        else
            puts("NO");
    }
    return 0;
}
