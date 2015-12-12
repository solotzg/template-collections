class Solution {
public:
    typedef vector<int> VI;
    typedef vector<VI> VVI;
    int largestRectangleArea(VI & h) {
        int c = h.size();
        VI sh(c+1), sw(c+1);
        int top = 0, w, res = 0;
        for (int j = 1, b; j<= c; ++j) {
            b = h[j-1];
            if (!top || b>=sh[top-1])
                sh[top] = b, sw[top] = 1, top++;
            else {
                w = 0;
                while (top && b <= sh[top-1]) {
                    w += sw[top-1];
                    res = max(res, w*(sh[--top]));
                }
                sh[top] = b, sw[top] = w+1, top++;
            }
        }
        for (w = 0; top; ) {
            w += sw[top-1];
            res = max(res, w*sh[--top]);
        }
        return res;
    }
};
