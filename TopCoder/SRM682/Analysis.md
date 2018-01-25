### https://community.topcoder.com/stat?c=round_overview&er=5&rd=16652

### SmilesTheFriendshipUnicorn
* 直接暴力深搜就可以，不知道为何值300分

### SuccessfulMerger
* 找规律，比赛的时候就差一点，tmd
* 根据题意，图的描述为一棵树加一条边，存在两种情况有一个环或无环
* 除了度为1的点外，其他点都可能被合并
* 对于非环上的点，合并方式就一种，不难看出来
* 环上的点需要分两种情况考虑，设环上有n个点，度>=3的点的数量为m
* 如果m==n，那么这n个点必然被合并成一个点
* 如果m!=n，那么其中一个度等于2的点可以不用合并

### FriendlyRobot
* dp[from][rest] 代表从字符串下标from开始，剩余可改变rest的最大值
* 依次枚举下次回到原点的下标
* 设当前的坐标为(0,0)经过指令序列from 到 next后得到坐标为(x,y)
* 只要满足abs(x)+abs(y)被2整除且小于等于rest的一半
* 设 x=3 y=1 X = abs(x) Y = abs(y) (X+Y) % 2 == 0
* 指令序列中必然包括X个R Y个U
* 那么只要将后(X+Y)/2个改成与前(X+Y)/2个对称
