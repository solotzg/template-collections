import java.util.ArrayList;
import java.util.LinkedList;

/**
 * Created by Zhigao on 2015/9/8.
 */
public class WalkOverATree {
    ArrayList<LinkedList<Integer>> edge = null;
    int dfs(int u, int pre) {
        int res = -1;
        for (int a : edge.get(u)) {
            if (a != pre) {
                res = Math.max(res, dfs(a, u));
            }
        }
        return res+1;
    }
    public int maxNodesVisited(int[] parent, int L) {
        int node = parent.length+1;
        int len = node-1;
        edge = new ArrayList<>();
        for (int i = 0; i< node; ++i)
            edge.add(new LinkedList<>());
        for (int i = 0; i< node-1; ++i) {
            edge.get(i+1).add(parent[i]);
            edge.get(parent[i]).add(i+1);
        }
        int deep = dfs(0, -1);
        if (deep >= L)
            return 1+L;
        if (deep + (len-deep)*2 <= L)
            return len+1;
        if ((L-deep)%2 != 0) {
            --deep;
        }
        return deep+(L-deep)/2+1;
    }
    public static void main(String []args) {
        WalkOverATree walkOverATree = new WalkOverATree();
        System.out.print(walkOverATree.maxNodesVisited(
                new int[]{0,0,0,1,1,3,5,1,4,5,2,2,10,5,10,10,11,13,8,3,18,15,20,20,23,8,11,26,4},
                26));
    }
}
