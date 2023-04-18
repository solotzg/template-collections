#pragma once

#include "utils/utils.h"

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