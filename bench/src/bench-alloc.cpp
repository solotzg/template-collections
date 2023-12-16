#include <bench/bench.h>

namespace {

template <typename Alloc> static void bench_alloc_impl(const size_t n) {
  Alloc alloc{};

  const auto label =
      fmt::format("{}, object-size: {}", Alloc::Label, Alloc::ObjSize);

  utils::TimeCost time_cost(label, false);
  rp(i, n) {
    auto p = alloc.New();
    alloc.Del(p);
  }
  auto dur = time_cost.Duration();
  time_cost.Show();
  bench::ShowDurAvgAndOps(dur, n);
}

template <typename T> struct STL_ALLOC {
  static constexpr size_t ObjSize = sizeof(T);
  static constexpr std::string_view Label = "STL";

  NO_INLINE T *New() { return new T(); }
  NO_INLINE void Del(T *p) { delete p; }
};

template <typename T> struct FAST_BIN_ALLOC : utils::FastBin<T> {
  using Base = utils::FastBin<T>;
  static constexpr std::string_view Label = "FAST_BIN";

  static constexpr size_t ObjSize = Base::OBJ_SIZE;
  NO_INLINE T *New() {
    auto p = Base::Alloc();
    return reinterpret_cast<T *>(p);
  }
  NO_INLINE void Del(T *p) { Base::Dealloc(p); }
};

#define DISPATCH(M)                                                            \
  M(128)                                                                       \
  M(256)                                                                       \
  M(512)                                                                       \
  M(1024)                                                                      \
  M(2048)                                                                      \
  M(4096)                                                                      \
  M(8192)

#define BENCH_ALLOC(obj_align, alloc_type)                                     \
  static void CONCAT(alloc_type, obj_align)(const size_t n) {                  \
    struct TestNode {                                                          \
      utils::AlignedStruct<double, obj_align> data;                            \
    };                                                                         \
    bench_alloc_impl<alloc_type<TestNode>>(n);                                 \
  }

#define M(obj_align)                                                           \
  BENCH_ALLOC(obj_align, STL_ALLOC) BENCH_ALLOC(obj_align, FAST_BIN_ALLOC)
DISPATCH(M)
#undef M

#undef BENCH_ALLOC

static void bench_alloc(int argc, char **argv) {

  const size_t n = 1e8;

  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";

  auto &&fn_bench_stl = [&] {
#define M(obj_align) CONCAT(STL_ALLOC, obj_align)(n);
    DISPATCH(M)
#undef M
  };
  auto &&fn_bench_fast_bin = [&] {
#define M(obj_align) CONCAT(FAST_BIN_ALLOC, obj_align)(n);
    DISPATCH(M)
#undef M
  };

#define M(name) fn_bench_##name();
  bench::FuncMap data = {
      {"ALL",
       [&] {
         M(stl)
         M(fast_bin)
       }},
      {"STL", [&] { M(stl) }},
      {"FAST_BIN", [&] { M(fast_bin) }},
  };
#undef M

  bench::ExecFuncMap(data, mode_str);
}

} // namespace

FUNC_FACTORY_REGISTER("ALLOC", bench_alloc)
