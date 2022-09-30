#pragma once

#include <cstdint>
#include <vector>

#define CONCAT_(prefix, suffix) prefix##suffix
#define CONCAT(prefix, suffix) CONCAT_(prefix, suffix)

#define rep(i, a, b)                                                           \
  for (int i = (a), CONCAT(__##i##_, __LINE__) = (b);                          \
       i != CONCAT(__##i##_, __LINE__); ++i)
#define urp(i, a, b)                                                           \
  for (int i = (a), CONCAT(__##i##_, __LINE__) = (b);                          \
       i != CONCAT(__##i##_, __LINE__); --i)
#define rp(i, b) rep(i, 0, b)
#define repd(i, a, b) rep(i, a, (b) + 1)
#define mst(a, b) memset(a, b, sizeof(a))
#define vtr(v) (v).begin(), (v).end()
#define mp(a, b) make_pair(a, b)
#define pb(a) push_back(a)
#define _0(x) (!(x))
#define _1(x) (x)
#define bit(x, y) (((x) >> (y)) & 1)

template <typename T> inline void smin(T &x, const T &y) { x = std::min(x, y); }
template <typename T> inline void smax(T &x, const T &y) { x = std::max(x, y); }
typedef int64_t LL;
typedef uint64_t ULL;
typedef std::vector<int> VI;
typedef std::vector<LL> VLL;
typedef std::vector<ULL> VULL;
typedef std::pair<int, int> PII;
typedef std::vector<PII> VPII;

#ifdef __GNUC__

typedef unsigned __int128 UINT128;
typedef __int128 INT128;

#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define FLATTEN_INLINE_PURE static __attribute__((flatten, always_inline, pure))
#define ALWAYS_INLINE static __attribute__((always_inline))
