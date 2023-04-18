#include "bench/bench.h"

namespace bench {

template <typename Alloc>
static void bench_alloc_impl(const size_t n, std::string_view name) {
  Alloc alloc{};

  const auto label = fmt::format("{}", name);

  utils::TimeCost time_cost(label, false);
  rp(i, n) {
    auto p = alloc.New();
    alloc.Del(p);
  }
  auto dur = time_cost.Duration();
  time_cost.Show();
  FMSGLN(std::locale("en_US.UTF-8"),
         "    count: {:L}, object-size: {}, avg: {}, qps: {:.3f}", n,
         Alloc::ObjSize, dur / n, n / (dur.count() / 1e9));
}

template <typename T> struct STL_ALLOC {
  static constexpr size_t ObjSize = sizeof(T);
  NO_INLINE T *New() { return allocator_.allocate(1); }
  NO_INLINE void Del(T *p) { allocator_.deallocate(p, 1); }
  std::allocator<T> allocator_{};
};

template <typename T> struct FAST_BIN_ALLOC : utils::FastBin<T> {
  using Base = utils::FastBin<T>;

  static constexpr size_t ObjSize = Base::OBJ_SIZE;
  NO_INLINE T *New() {
    auto p = Base::Alloc();
    return reinterpret_cast<T *>(p);
  }
  NO_INLINE void Del(T *p) { Base::Dealloc(p); }
};

static void bench_alloc(int argc, char **argv) {

  const size_t n = 1e8;

  struct TestNode {
    utils::AlignedStruct<double, 128> data;
  };

  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";

  auto &&fn_bench_stl = [&] {
    bench_alloc_impl<STL_ALLOC<TestNode>>(n, "STL");
  };
  auto &&fn_bench_fast_bin = [&] {
    bench_alloc_impl<FAST_BIN_ALLOC<TestNode>>(n, "FAST_BIN");
  };

#define M(name) fn_bench_##name();
  if (mode_str == "ALL") {
    M(stl)
    M(fast_bin)
  } else if (mode_str == "STL") {
    M(stl)
  } else if (mode_str == "FAST_BIN") {
    M(fast_bin)
  }
#undef M
}

} // namespace bench

FUNC_FACTORY_REGISTER("ALLOC", bench::bench_alloc)
