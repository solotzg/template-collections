#include <bench/bench.h>
#include <utils/timer.hpp>
#include <utils/utils.h>

#ifndef NDEBUG
constexpr bool kCheckConsistency = true;
#else
constexpr bool kCheckConsistency = false;
#endif

namespace {

NO_INLINE void run_stl(char *p, utils::SysMilliseconds &last_update_time_sec) {
  utils::SerializeTimepoint(p, last_update_time_sec, DEF_SYSTEM_CLOCK_NOW);
}
NO_INLINE void run_async(char *p,
                         utils::SysMilliseconds &last_update_time_sec) {
  utils::SerializeTimepoint(p, last_update_time_sec, ASYNC_SYSTEM_CLOCK_NOW);
}

static void bench_async_timepoint(int argc, char **argv) {
#ifdef NDEBUG
  const size_t n = 1e7;
#else
  const size_t n = 20;
#endif
  std::array<char, utils::kLogTimePointSize> buff;
  utils::SysMilliseconds last_update_time_sec{};

#define F(name, label)                                                         \
  auto CONCAT(fn_bench_, name) = [&]() {                                       \
    utils::TimeCost tc(#label, false);                                         \
    rp(i, n) {                                                                 \
      CONCAT(run_, name)(buff.data(), last_update_time_sec);                   \
      DEBUG_MSGLN(std::string_view{buff.data(), buff.size()});                 \
      DEBUG_SCOPE({ std::this_thread::sleep_for(utils::Milliseconds{1}); });   \
    }                                                                          \
    auto &&dur = tc.Duration();                                                \
    tc.Show();                                                                 \
    bench::ShowDurAvgAndOps(dur, n);                                           \
  };
  F(stl, STL);
  F(async, ASYNC);
#undef F

  std::string mode_str = argc > 0 ? utils::ToUpper(argv[0]) : "ALL";

#define M(name) fn_bench_##name();
  bench::FuncMap data = {
      {"ALL",
       [&] {
         M(stl)
         M(async)
       }},
      {"ASYNC", [&] { M(async) }},
      {"STL", [&] { M(stl) }},
  };
#undef M

  bench::ExecFuncMap(data, mode_str);
}

FUNC_FACTORY_REGISTER("ASYNC_TIMEPOINT", bench_async_timepoint)

} // namespace