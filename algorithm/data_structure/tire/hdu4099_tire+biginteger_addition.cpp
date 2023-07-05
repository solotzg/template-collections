#include <algorithm>
#include <cstdio>
#include <cstring>
using namespace std;
#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vrp(it, v) for (auto it(v.begin()); (it) != (v.end()); ++it)
struct Tire {
  const static int S = 10;
  struct Node {
    Node *son[S];
    int b_end;
    void init() {
      memset(son, 0, sizeof(son));
      b_end = -1;
    }
  };
  Node *root;
  void init() {
    root = new Node;
    root->init();
  }
  void insert(const char *s, int len, int idx) {
    int ta;
    Node *p = root;
    for (int i = 0; i < len && i < 41; ++i) {
      ta = s[i] - '0';
      if (!p->son[ta])
        p->son[ta] = new Node, p->son[ta]->init(), p->son[ta]->b_end = idx;
      p = p->son[ta];
    }
  }
  int b_find(const char *s, int len) {
    int o;
    Node *p = root;
    for (int i = 0; i < len; ++i) {
      o = s[i] - '0';
      if (!p->son[o])
        return -1;
      p = p->son[o];
    }
    return p->b_end;
  }
};
struct Num {
  const static int N = 80;
  char pre[N];
  int w;
};
void add(Num &a, Num &b, Num &c) {
  static char tp[80];
  int i = a.w - 1, j = b.w - 1, k = 0;
  int o, p = 0, t1, t2;
  while (i >= 0 || j >= 0) {
    t1 = i >= 0 ? a.pre[i] - '0' : 0;
    t2 = j >= 0 ? b.pre[j] - '0' : 0;
    o = t1 + t2 + p;
    tp[k++] = o % 10 + '0';
    p = o / 10;
    --i, --j;
  }
  if (p)
    tp[k++] = p + '0';
  for (int i = 0; i < k; ++i)
    c.pre[i] = tp[k - 1 - i];
  c.w = k;
}
Tire a;
void init() {
  int m1 = 0, m2 = 1, m3 = 2, tp;
  a.init();
  Num n[3];
  n[0].pre[0] = '1';
  n[1].pre[0] = '1';
  n[0].w = n[1].w = 1;
  a.insert(n[0].pre, 1, 0);
  for (int i = 2; i < 100000; ++i) {
    if (n[m2].w > 60)
      --n[m1].w, --n[m2].w;
    add(n[m1], n[m2], n[m3]);
    a.insert(n[m3].pre, n[m3].w, i);
    tp = m1;
    m1 = m2;
    m2 = m3;
    m3 = tp;
  }
}
char sp[50];
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif
  int t;
  scanf("%d", &t);
  init();
  for (int i = 1; i <= t; ++i) {
    printf("Case #%d: ", i);
    scanf("%s", sp);
    printf("%d\n", a.b_find(sp, strlen(sp)));
  }
  return 0;
}
