#include<cstdio>
#include<iostream>
#include<string>
#include<cstring>
#include<algorithm>
#include<cmath>
#include<vector>
using namespace std;
const int MAXC = 330;
const int MAXR = 735;
const int MAXM = MAXR*MAXC;
int res[100], check[MAXC], nr;
struct DLX {
    int L[MAXM],R[MAXM],U[MAXM],D[MAXM];
    int sz,row[MAXM],col[MAXM],S[MAXC],H[MAXR];
    void del(int c) {
        L[R[c]]=L[c];
        R[L[c]]=R[c];
        for(int i=D[c]; i!=c; i=D[i])
            for(int j=R[i]; j!=i; j=R[j])
                U[D[j]]=U[j],D[U[j]]=D[j],--S[col[j]];
    }
    void add(int c) {
        R[L[c]]=L[R[c]]=c;
        for(int i=U[c]; i!=c; i=U[i])
            for(int j=L[i]; j!=i; j=L[j])
                ++S[col[U[D[j]]=D[U[j]]=j]];
    }
    void init(int m) {
        for(int i=0; i<=m; i++) {
            S[i]=0;
            L[i]=i-1;
            R[i]=i+1;
            U[i]=D[i]=i;
        }
        L[0]=m;
        R[m]=0;
        sz=m+1;
        memset(H,-1,sizeof(H));
    }
    void link(int x,int y) {
        ++S[col[sz]=y];
        row[sz]=x;
        D[sz]=D[y];
        U[D[y]]=sz;
        U[sz]=y;
        D[y]=sz;
        if(H[x]<0)H[x]=L[sz]=R[sz]=sz;
        else {
            R[sz]=R[H[x]];
            L[R[H[x]]]=sz;
            L[sz]=H[x];
            R[H[x]]=sz;
        }
        sz++;
    }
    bool dfs() {
        if(!R[0])
            return 1;
        int c=R[0];
        for(int i=R[0]; i; i=R[i])if(S[c]>S[i])c=i;
        del(c);
        for(int i=D[c]; i!=c; i=D[i]) {
            res[nr++] = i;
            for(int j=R[i]; j!=i; j=R[j])del(col[j]);
            if(dfs())
                return 1;
            for(int j=L[i]; j!=i; j=L[j])add(col[j]);
            --nr;
        }
        add(c);
        return 0;
    }
} dlx;
char mmp[20][20];
int main() {
#ifndef ONLINE_JUDGE
    freopen("in.txt", "r", stdin);
    //freopen("out.txt", "w", stdout);
#endif
    int T, ans;
    scanf("%d", &T);
    for (int t = 0; t< T; ++t) {
        ans = 1;
        nr = 0;
        memset(check, 0, sizeof check);
        if (t) scanf("%s", mmp[0]), printf("---\n");
        dlx.init(9*9*4);
        memset(res, -1, sizeof res);
        for (int i = 1, k = 1, p, u, r, c; i<= 9; ++i) {
            scanf("%s", mmp[i]+1);
            for (int j = 1; j<= 9; ++j, ++k)
                if (mmp[i][j] != '?') {
                    p = mmp[i][j] - '0';
                    r = (i-1)/3+1;
                    c = (j-1)/3+1;
                    c = (r-1)*3+c;
                    dlx.link(u=(k-1)*9+p, k);
                    dlx.link(u, r=(i-1)*9+p+81);
                    if (check[r])
                        ans = 0;
                    check[r] = 1;
                    dlx.link(u, r=(j-1)*9+p+162);
                    if (check[r])
                        ans = 0;
                    check[r] = 1;
                    dlx.link(u, r=(c-1)*9+p+243);
                    if (check[r])
                        ans = 0;
                    check[r] = 1;
                } else {
                    for (p = 1; p <= 9; ++p) {
                        r = (i-1)/3+1;
                        c = (j-1)/3+1;
                        c = (r-1)*3+c;
                        dlx.link(u=(k-1)*9+p, k);
                        dlx.link(u, (i-1)*9+p+81);
                        dlx.link(u, (j-1)*9+p+162);
                        dlx.link(u, (c-1)*9+p+243);
                    }
                }
        }
        if (!ans || !dlx.dfs())
            printf("impossible\n");
        else {
            for (int i = 0, o; i< nr; ++i) {
                o = dlx.row[res[i]];
                int r = (o-1)/81+1;
                int c = (o-1)%81/9+1;
                int p = (o-1)%9+1;
                mmp[r][c] = '0'+p;
            }
            for (int i = 1; i<= 9; ++i)
                printf("%s\n", mmp[i]+1);
        }
    }
    return 0;
}
