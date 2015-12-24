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
