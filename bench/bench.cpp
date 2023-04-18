#include "bench/bench.h"

namespace bench {

void FuncFactory::Register(std::string name, Func &&func) {
  RunWithMutexLock([&] {
    auto [it, ok] = data_.emplace(std::move(name), std::move(func));
    RUNTIME_ASSERT(ok);
  });
}

std::string FuncFactory::HelpMessage() {
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

void FuncFactory::Run(int argc, char **argv) {
  RunWithMutexLock([&] {
    if (argc == 0 || !Run(argv[0], argc - 1, &argv[1])) {
      FMSGLN("HELP:\n    {}", HelpMessage());
    }
  });
}

FuncFactory &FuncFactory::instance() {
  static FuncFactory fac;
  return fac;
}

bool FuncFactory::Run(std::string_view n, int argc, char **argv) {
  auto name = utils::ToUpper(n);
  if (auto it = data_.find(name); it != data_.end()) {
    LOG_INFO("Start to run `{}`", name);
    utils::TimeCost time_cost{name, false};
    it->second(argc, argv);
    LOG_INFO(
        "`{}` costs {}", name,
        std::chrono::duration_cast<utils::Milliseconds>(time_cost.Duration()));
    return true;
  }
  return false;
}

} // namespace bench

int main(int argc, char **argv) {
  utils::ShowBuildInfo(std::cout);
  MSGLN("------");
  bench::FuncFactory::instance().Run(argc - 1, &argv[1]);
  return 0;
}