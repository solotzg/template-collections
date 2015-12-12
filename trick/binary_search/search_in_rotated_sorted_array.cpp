// https://leetcode.com/problems/search-in-rotated-sorted-array/
#include<bits/stdc++.h>
using namespace std;
class Solution {
public:
    int search(vector<int>& nums, int t) {
        int len = nums.size();
        int bg, ed;
        bg = 0, ed = len-1;
        while (bg < ed) {
            int m = (bg+ed)/2;
            int k = nums[m], b = nums[bg], e = nums[ed];
            if (k == t) return m;
            if (b < e) {
                if (k < t) bg = m+1;
                else ed = m-1;
            } else {
                if (k >= b) {
                    if (t >= b && t <= k) {
                        ed = m-1;
                    } else {
                        bg = m+1;
                    }
                } else {
                    if (t >= k && t <= e)
                        bg = m+1;
                    else
                        ed = m-1;
                }
            }
        }
        return nums[bg]==t?bg:-1;
    }
};
int main() {
    
    return 0;
}
