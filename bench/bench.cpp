#include <bench/bench.h>
#include <utils/timer.hpp>

namespace bench {

void FuncFactory::Register(std::string name, Func &&func) {
  utils::ToUpper(name);
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

void ShowDurAvgAndOps(const utils::TimeCost::Clock::duration &dur, U64 n,
                      U64 clock_counter) {
  auto &&[avg, ops] = DurAvgAndOps(dur, double(n));
  std::string extra;
  if (clock_counter != -1) {
    extra = fmt::format(", avg-clock-count: {:.3f}", double(clock_counter) / n);
  }
  FMSGLN("    count: {}, avg: {}, ops: {:.3f}{}",
         fmt::format(std::locale("en_US.UTF-8"), "{:L}", n), avg, ops, extra);
}

void ExecFuncMap(FuncMap &data, const std::string &fname) {
  if (auto it = data.find(fname); it != data.end()) {
    it->second();
  } else {
    FMSGLN("Unknown func `{}`, option:", fname);
    for (auto &&e : data) {
      FMSGLN("  {}", e.first);
    }
  }
}

} // namespace bench

int main(int argc, char **argv) {
  utils::ShowBuildInfo(std::cout);
  MSGLN("------");
  bench::FuncFactory::instance().Run(argc - 1, &argv[1]);
  return 0;
}