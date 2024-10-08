#pragma once

#include <utils/utils.h>

namespace bench {

struct FuncFactory : utils::MutexLockWrap {
  using Func = void (*)(int, char **);

  void Register(std::string name, Func &&func);

  std::string HelpMessage();

  void Run(int argc, char **argv);

  static FuncFactory &instance();

protected:
  bool Run(std::string_view n, int argc, char **argv);

protected:
  std::unordered_map<std::string, Func> data_;
};

inline auto DurAvgAndOps(const utils::TimeCost::Clock::duration &dur,
                         double n) {
  auto &&avg = dur / n;
  auto &&ops = utils::Seconds{1} / avg;
  return std::make_pair(avg, ops);
}

void ShowDurAvgAndOps(const utils::TimeCost::Clock::duration &dur, U64 n,
                      U64 clock_counter = -1);

using FuncMap = std::unordered_map<std::string, std::function<void()>>;

void ExecFuncMap(FuncMap &, const std::string &);

} // namespace bench

#define FUNC_FACTORY_REGISTER(name, func)                                      \
  namespace {                                                                  \
  struct BenchFuncHolder {                                                     \
    BenchFuncHolder() {                                                        \
      bench::FuncFactory::instance().Register((name), (func));                 \
    }                                                                          \
  };                                                                           \
  static auto CONCAT(local_bench_func_holder, __LINE__) = BenchFuncHolder();   \
  }
