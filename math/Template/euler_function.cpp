const static int M = 1e7;
int phi[M+1], vis[M+1], prime[M], np;
void init() {
    phi[1] = 1;
    repd(i,2,M) {
        if (!vis[i]) {
            prime[np++] = i;
            phi[i] = i-1;
        }
        for (int j = 0, tp; j<np && (ll)prime[j]*i<=M; ++j) {
            tp = prime[j]*i;
            vis[tp] = 1;
            if (i % prime[j] == 0) {
                phi[tp] = prime[j]*phi[i];
                break;
            } else {
                phi[tp] = phi[i]*(prime[j]-1);
            }
        }
    }
}
