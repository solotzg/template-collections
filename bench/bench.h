#pragma once

#include "utils/utils.h"

namespace bench {

struct FuncFactory : utils::MutexLockWrap {
  using Func = void (*)(int, char **);

  void Register(std::string name, Func &&func) {
    RunWithMutexLock([&] { data_.emplace(std::move(name), std::move(func)); });
  }

  std::string HelpMessage() {
    fmt::memory_buffer out{};
    {
      SCOPE_EXIT_FMT_SURROUND(out, "{", "}");
      size_t i = 0;
      for (auto &&e : data_) {
        FMT_IF_APPEND(out, i, ", ");
        FMT_APPEND(out, "{}", e.first);
        ++i;
      }
    }
    return fmt::to_string(out);
  }

  void Run(int argc, char **argv) {
    bool run_all = argc == 0;
    RunWithMutexLock([&] {
      if (run_all) {
        for (auto &&[name, func] : data_) {
          auto res = Run(name, 0, nullptr);
          RUNTIME_ASSERT(res);
        }
      } else {
        auto res = Run(argv[0], argc - 1, &argv[1]);
        if (!res) {
          FMSGLN("HELP:\n    {}", HelpMessage());
        }
      }
    });
  }

  static FuncFactory &instance() {
    static FuncFactory fac;
    return fac;
  }

protected:
  bool Run(std::string_view n, int argc, char **argv) {
    auto name = utils::ToUpper(n);
    if (auto it = data_.find(name); it != data_.end()) {
      LOG_INFO("Start to run `{}`", name);
      utils::TimeCost time_cost{name, false};
      it->second(argc, argv);
      LOG_INFO("`{}` costs {}", name,
               std::chrono::duration_cast<utils::Milliseconds>(
                   time_cost.Duration()));
      return true;
    }
    return false;
  }

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