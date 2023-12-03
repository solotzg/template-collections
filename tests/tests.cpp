#include <tests/tests.h>
#include <utils/async-log.hpp>
#include <utils/async.hpp>
#include <utils/mpsc.hpp>

namespace tests {

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
    if (argc == 0 || !Run(argv[0])) {
      FMSGLN("HELP:\n    {}", HelpMessage());
    }
  });
}

FuncFactory &FuncFactory::instance() {
  static FuncFactory fac;
  return fac;
}

bool FuncFactory::Run(std::string_view n) {
  auto name = utils::ToUpper(n);
  if (auto it = data_.find(name); it != data_.end()) {
    LOG_INFO("Start to run `{}`", name);
    utils::TimeCost time_cost{name, false};
    it->second();
    LOG_INFO(
        "`{}` costs {}", name,
        std::chrono::duration_cast<utils::Milliseconds>(time_cost.Duration()));
    return true;
  } else if (name == "ALL") {
    for (auto &&p : data_) {
      if (auto ok = Run(p.first); !ok) {
        return false;
      }
    }
    return true;
  }
  return false;
}

} // namespace tests

int main(int argc, char **argv) {
  utils::ShowBuildInfo(std::cout);
  MSGLN("------");

  tests::FuncFactory::instance().Run(argc - 1, &argv[1]);
  return 0;
}
