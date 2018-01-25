#include<bits/stdc++.h>
using namespace std;

const int N = 55;
int fa[N];
int find(int x) {
    return fa[x] = fa[x]==x?x:find(fa[x]);
}
struct ChristmasTreeDecoration {
    int solve(vector <int> col, vector <int> x, vector <int> y) {
        int f = 1;
        int res = 0;
        for (int i = 0; i<N; ++i) fa[i] = i;
        while (f) {
            f = 0;
            int ta = -1,  tb = -1;
            int qa = -1, qb = -1;
            for (int i = 0; i< x.size(); ++i) {
                int a = x[i], b = y[i];
                --a, --b;
                if (find(a) != find(b)) {
                    if (col[a] == col[b]) {
                        ta = a, tb = b;
                    } else {
                        qa = a, qb = b;
                    }
                }
            }
            if (qa != -1) {
                fa[ find(qa) ] = find(qb);
                f = 1;
            } else if (ta != -1) {
                f = 1;
                fa[ find(ta) ] = find(tb);
                ++res;
            }
        }
        return res;
    }
};
