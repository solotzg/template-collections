#include<bits/stdc++.h>
using namespace std;

struct CartInSupermarket{
	int calcmin(vector <int> a, int b);
};

int h(int x , int s) {
	if (s == 0) return x;
	int time = 0;
	int c = 1;
	for ( ; c<=s ; ) {
		s -= c;
		c <<= 1;
		++time;
	}
	++time;
	x = (-c+s+x);
	int u = c+s;
	return time+(x+u-1)/(u);
}

int g(int x, int c) {
	int bg = 0, ed = x-1;
	while (bg < ed) {
		int m = (bg+ed)/2;
		if (h(x, m) <= c) ed = m;
		else bg = m+1;
	}
	return bg;
}

int f(const vector<int> & a, int b, int c) {
	int r = 0;
	for (int e  : a) {
		int o = 0, n = e;
		while (n) o++, n >>=1;
		if (o > c) return 0;
		r += g(e, c);
		if (r > b) return 0;
	}
	return 1;
}
int CartInSupermarket::calcmin(vector <int> a, int b){
	int bg = 1, ed = 1e9;
	while (bg < ed) {
		int m = (bg + ed)/2;
		if (f(a, b, m)) ed = m;
		else bg = m+1;
	}
	return bg;
}
