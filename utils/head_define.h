#include <cstdint>
#include <vector>

#define rep(i, a, b) for (int i = (a), __tzg_##i = (b); i < __tzg_##i; ++i)
#define urp(i, a, b) for (int i = (a), __tzg_##i = (b); i >= __tzg_##i; --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vtr(v) (v).begin(), (v).end()
#define mp(a, b) make_pair(a, b)
#define pb(a) push_back(a)
#define _0(x) (!(x))
#define _1(x) (x)
#define bit(x, y) (((x) >> (y)) & 1)
#define siz(x) ((int)(x).size())
template <typename T> inline void smin(T &x, const T &y) { x = std::min(x, y); }
template <typename T> inline void smax(T &x, const T &y) { x = std::max(x, y); }
typedef int64_t LL;
typedef uint64_t ULL;
typedef std::vector<int> VI;
typedef std::vector<LL> VLL;
typedef std::vector<ULL> VULL;
typedef std::pair<int, int> PII;
typedef std::vector<PII> VPII;