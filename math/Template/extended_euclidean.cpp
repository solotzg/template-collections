#include <cstdio>
#include <complex>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
using namespace std;

int extend_gcd(int a,int b,int &x,int &y) {
    if(b==0) {
        x=1;
        y=0;
        return a;
    } else {
        int r = extend_gcd(b,a%b,x,y);
        int t=x;
        x=y;
        y=t-(a/b)*y;
        return r;
    }
}
// http://www.cnblogs.com/frog112111/archive/2012/08/19/2646012.html
int main() {
    {
        /**
            乘法逆元
        */
        int a = 2, b = 5, x, y;
        extend_gcd(a, b, x, y);
        x = (x%b+b)%b;
        printf("%d\n", x);
    }
    {
        /**
            gcd
        */
        int a = 20, b = 28, x, y;
        int r = extend_gcd(a, b, x, y);
        printf("%d\n", r);
    }
    {
        /**
            用扩展欧几里得算法解不定方程ax+by=c;
            gcd(a,b) % c == 0;
        */
        int a = 4, b = 6, c = 8, x, y;
        int r = extend_gcd(a,b,x,y);
        printf("%d*%d + %d*%d = %d\n", a, x*c/r, b, y*c/r, c);
    }
    return 0;
}
