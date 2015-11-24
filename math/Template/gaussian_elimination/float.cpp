#include <cmath>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <iostream>
using namespace std;
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
} _gauss;
