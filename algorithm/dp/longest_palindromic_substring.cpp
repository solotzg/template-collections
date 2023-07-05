// Longest Palindromic Substring
#include <bits/stdc++.h>
using namespace std;
class LongestPalindromicSubstring {
public:
  const int INF = 0x3f3f3f3f;
  string longestPalindrome(string s) {
    int ls = s.size(), lc = ls * 2 + 1;
    if (!ls)
      return "";
    string c(lc, 0);
    vector<int> mx(lc, INF);
    for (int i = 0; i < ls; ++i)
      c[1 + i * 2] = s[i];
    int rmx = 1, rdx = 0;
    int cmx = 1, cdx = 0;
    mx[0] = 1;
    for (int i = 1; i < lc; ++i) {
      int k = cmx + cdx - 1;
      int ndx = i, nmx = 1;
      if (i <= k)
        nmx = min(mx[cdx * 2 - i], k - i + 1);
      for (int r = ndx + nmx, l = ndx * 2 - r; r < lc && l >= 0 && c[l] == c[r];
           ++r, --l, ++nmx)
        ;
      if (ndx + nmx > k + 1)
        cdx = ndx, cmx = nmx;
      mx[ndx] = nmx;
      if (rmx < nmx)
        rmx = nmx, rdx = ndx;
    }
    string t = "";
    for (int r = rdx + rmx - 1, l = rdx * 2 - r; l <= r; ++l)
      if (c[l])
        t += c[l];
    return t;
  }
};
int main() {
  LongestPalindromicSubstring *test1 = new LongestPalindromicSubstring();
  cout << test1->longestPalindrome("1233210012334") << endl;
  return 0;
}
