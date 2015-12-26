/*
 * A、B是非负整数，p是质数。AB写成p进制：A=a[n]a[n-1]...a[0]，B=b[n]b[n-1]...b[0]
 * 则组合数C(A,B)与C(a[n],b[n])*C(a[n-1],b[n-1])*...*C(a[0],b[0])  mod p 同余
 * 即：Lucas(n,m,p)=c(n%p,m%p)*Lucas(n/p,m/p,p) 
 * */
#include<cstdio>
#include<iostream>
#include<string>
#include<cstring>
#include<algorithm>
#include<cmath>
#include<vector>
#include<set>
#include<queue>
using namespace std;
#define rep(i,a,b) for(int i(a), __tzg##i(b);i<__tzg##i;++i)
#define rp(i,b) rep(i,0,b)
#define repd(i,a,b) rep(i,a,1+b)
#define urp(i,a,b) for(int i(a), __tzg##i(b);i>=__tzg##i;--i)
typedef long long ll;
const static int N = 100005;
ll factorial[N];
void init(ll mod) {
    factorial[0] = 1;
    rep(i,1,mod) factorial[i] = factorial[i-1]*i%mod;
}
ll pow(ll a, ll b, ll mod) {
    ll res = 1, p = a;
    while (b) {
        if (b & 1) res = res*p%mod;
        p = p*p%mod;
        b>>=1;
    }
    return res;
}
ll comb(ll m, ll n, ll mod) {
    return factorial[m]*pow(factorial[n]*factorial[m-n]%mod,mod-2,mod)%mod;
}
ll lucas(ll m, ll n, ll mod) {
    ll res = 1;
    while (m && n) {
        ll nn = n%mod, mm = m%mod;
        if (nn > mm) return 0;
        res = res*comb(mm, nn, mod)%mod;
        n /= mod;
        m /= mod;
    }
    return res;
}
int main(){
#ifndef ONLINE_JUDGE
    freopen("in.txt", "r", stdin);
#endif
    int t;
    ll n, m, p;
    cin>>t;
    while (t--) {
        cin>>n>>m>>p;
        init(p);
        cout<<lucas(n+m,m,p)<<endl;
    }
    return 0;
}
