### https://community.topcoder.com/stat?c=round_overview&rd=16552
### http://apps.topcoder.com/wiki/display/tc/SRM+672

### AlmostEulerianGraph
* Eulerian Graph means a undirected graph with no vertice owning odd degree
* First, count the number of graphs(connected or not) that have even degree: e(n)
* Second, count the number of disconnected graphs with even degree: d(n)
* Third, count the number of connected graphs with even degree: p(n)

#### Note
* There is no vertice forming sequence, so while transforming state, we need to specify a vertice such as v0

#### Even degree graphs
* Assume that there is n vertices, v0 and other n-1 ones. For vertice vi with odd degree in these n-1 vertices, we can add an edge between v0 and vi to make the degree even.
* There are 2^((n-1)(n-2)/2) ways to make up graphs with n-1 vertices. For each way, we can finally get even degree graphs with n vertices
* Assume x vertices have even degree, n-1-x vertices have odd degree.
* Sum(v1...vn-1 degree) must be even. So n-1-x must be even

#### Disconnected even degree graphs
* Assume that there is n vertices, v0 and other n-1 ones, and x of n-1 vertices and v0 form a connected graph. So there are d(n) = Comb(n-1,x)\*p(x+1)\*2^((n-1-x)\*(n-2-x)/2) ways to make up disconnected even degree graphs.
* p(n) = e(n) - d(n)

### Tdetective
* it's hard to explain in English, so Chinese

#### 题意
* n个嫌疑犯，编号为0～n-1，其中一个为杀人犯，每个嫌犯有对所有嫌犯嫌疑等级的评定（0~9），编号为0的是首要嫌犯。
* 1个侦探，和嫌疑犯谈话即可判断其是否为杀人犯，如果非杀人犯，则可从其口中得知的信息更新嫌疑等级（最大值），否则停止
* 求出当k为杀人犯时，最少需要多少次谈话

####
* 可以模拟一下，最先调查0，然后更新嫌疑等级，再挑等级最大的进行下一次调查，如此循环。
* 所以有个很trick的办法，从9到0枚举等级，利用并查集把连续调查的人添加到同一集合之中。为什么要从9到0枚举？因为高等级的总是会被优先调查。
* 设当前枚举的等级为i，维护一个数组r(c,b)表示调查过c后，调查b需要的次数。
* 对于任意a、b，如果s(a,b)==i但a和b不再同一个集合中（两者互评没有更高的优先级），再枚举c，满足a和c在同一集合中，那么就可以更新r(c,b)=min(r(c,b),o(find(c)))
* ......
