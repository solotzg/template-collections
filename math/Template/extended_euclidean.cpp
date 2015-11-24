#include <cstdio>
#include <complex>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
using namespace std;
/*
    gcd(a,md) == 1
*/
void extend_gcd(int a,int md,int &x,int &y) {
    if(md==0) {
        x=1;
        y=0;
    } else {
        int t;
        extend_gcd(md,a%md,x,y);
        t=x;
        x=y;
        y=t-(a/md)*y;
    }
}
int main() {
    int a = 2, b = 1000000007, x, y;
    extend_gcd(a, b, x, y);
    x = (x%b+b)%b;
    printf("%d\n", a*x%b);
    return 0;
}
