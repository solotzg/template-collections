#include<cstdio>
#include<algorithm>
#include<cstring>
#include<iostream>
using namespace std;
template<typename T>
T gcd (T a, T b){
    while (b^=a^=b^=a%=b);
    return a;
}
int main() {
    cout<<gcd(20, 5)<<endl;
    cout<<gcd(3, 20)<<endl;
    cout<<gcd(1000000007ll, 1000000007ll*1000000007ll)<<endl;
    return 0;
}
