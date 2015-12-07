#include <cmath>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <iostream>
using namespace std;
int n;
double p;
struct Gauss {
    const static int MAXN = 300;
    const double _inf = 1e-9;
    double a[MAXN][MAXN], x[MAXN]; // 方程左边的矩阵和等式右边的值， x存放最后结果
    int equ, val;	// 方程数 未知数个数
    int gauss() {
        int i,j,k,col,max_r;
        for(k=0,col=0; k<equ&&col<val; k++,col++) {
            max_r=k;
            for(i=k+1; i<equ; i++)
                if(fabs(a[i][col])>fabs(a[max_r][col]))
                    max_r=i;
            if(fabs(a[max_r][col])<_inf)
                return 0;
            if(k!=max_r) {
                for(j=col; j<val; j++)
                    swap(a[k][j],a[max_r][j]);
                swap(x[k],x[max_r]);
            }
            x[k]/=a[k][col];
            for(j=col+1; j<val; j++)a[k][j]/=a[k][col];
            a[k][col]=1;
            for(i=0; i<equ; i++)
                if(i!=k) {
                    x[i]-=x[k]*a[i][k];
                    for(j=col+1; j<val; j++)a[i][j]-=a[k][j]*a[i][col];
                    a[i][col]=0;
                }
        }
        return 1;
    }
    void init() {
        memset(a, 0, sizeof a);
        memset(x, 0, sizeof x);
        equ = 291;
        val = 291;
        a[0][0] = 1.0;
        a[1][1] = 1.0;
        a[2][2] = 1.0;

        for (int i = 1, j, k = 3; i<97; ++i, k+=3) {
            a[k][3*i] = -1.0;
            a[k][3*(i-1)+1] = p;
            j = i>85?i:i+1;
            a[k][3*j] += 1-p;
            x[k] = -1.0;

            a[k+1][3*i+1] = -1.0;
            a[k+1][3*(i-1)+2] = p;
            j = i>85?i:i+1;
            a[k+1][3*j] += 1-p;
            x[k+1] = -1.0;

            a[k+2][3*i+2] = -1.0;
            j = i<=25?i-1:i-2;
            a[k+2][3*j+2] = p;
            j = i>85?i:i+1;
            a[k+2][3*j] += 1-p;
            x[k+2] = -1.0;
        }
    }
} _gauss;
int main() {
#ifndef ONLINE_JUDGE
    freopen("in.txt", "r", stdin);
#endif
    scanf("%d %lf", &n, &p);
    _gauss.init();
    int f = _gauss.gauss();
    if (f) {
        printf("%.9lf\n", _gauss.x[3*n]);
    }
    return 0;
}
