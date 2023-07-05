#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>
using namespace std;
#define rep(i, a, b) for (int i(a), __tzg##i(b); i < __tzg##i; ++i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, 1 + b)
#define urp(i, a, b) for (int i(a), __tzg##i(b); i >= __tzg##i; --i)
typedef long long ll;
const static int N = 100010;
struct FuncSegTree {
  // 区间第k大
  struct Node {
    int sum, lson, rson;
    Node(int _sum = 0, int _lson = 0, int _rson = 0)
        : sum(_sum), lson(_lson), rson(_rson) {}
  };
  Node tree[N * 20];
  int cnt;
  void init(int &root, int l, int r) {
    cnt = 0;
    build(root, l, r);
  }
  void build(int &root, int l, int r) {
    root = new_node(0, l, r);
    if (l == r)
      return;
    int m = (l + r) / 2;
    build(tree[root].lson, l, m);
    build(tree[root].rson, m + 1, r);
  }
  int new_node(int sum, int lson, int rson) {
    tree[++cnt] = Node(sum, lson, rson);
    return cnt;
  }
  void insert(int &root, int pre, int pos, int l, int r) {
    // 从根节点往下更新到叶子，新建立出一路更新的节点，这样就是一颗新树了。
    root = new_node(tree[pre].sum + 1, tree[pre].lson, tree[pre].rson);
    if (l == r)
      return;
    int m = (l + r) / 2;
    if (pos <= m)
      insert(tree[root].lson, tree[pre].lson, pos, l, m);
    else
      insert(tree[root].rson, tree[pre].rson, pos, m + 1, r);
  }
  int query(int s, int e, int k, int l, int r) {
    if (l == r)
      return l;
    int m = (l + r) / 2;
    // 下面计算的sum就是当前询问的区间中，左儿子中的元素个数
    int sum = tree[tree[e].lson].sum - tree[tree[s].lson].sum;
    if (k <= sum)
      return query(tree[s].lson, tree[e].lson, k, l, m);
    else
      return query(tree[s].rson, tree[e].rson, k - sum, m + 1, r);
  }
} func_seg_tree;
int nums[N], sorted[N], root[N];
int main() {
#ifndef ONLINE_JUDGE
  freopen("in.txt", "r", stdin);
#endif
  int t;
  cin >> t;
  while (t--) {
    int n, m, p = 0;
    cin >> n >> m;
    repd(i, 1, n) scanf("%d", nums + i), sorted[i] = nums[i];
    sort(sorted + 1, sorted + n + 1);
    p = unique(sorted + 1, sorted + n + 1) - (sorted + 1);
    func_seg_tree.init(root[0], 1, p);
    repd(i, 1, n) {
      int dx = lower_bound(sorted + 1, sorted + 1 + p, nums[i]) - sorted;
      func_seg_tree.insert(root[i], root[i - 1], dx, 1, p);
    }
    while (m--) {
      int l, r, u, o;
      scanf("%d%d%d", &l, &r, &u);
      o = func_seg_tree.query(root[l - 1], root[r], u, 1, p);
      printf("%d\n", sorted[o]);
    }
  }
  return 0;
}
