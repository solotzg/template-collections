### https://community.topcoder.com/stat?c=round_overview&er=5&rd=16625
### http://apps.topcoder.com/wiki/display/tc/SRM+675

#### TreeAndPathLength2
* 设点0为root，枚举添加i个点到root或其他叶子节点之下。
* 状态stat(n,p,s)。表示当前共n个点，叶子节点p个，长度为2的路径共s条
* 任意一个图形可按上述方法得到
