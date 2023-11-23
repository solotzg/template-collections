#pragma once

namespace tests {

struct FuncFactory : utils::MutexLockWrap {
  using Func = void (*)();

  void Register(std::string name, Func &&func);

  std::string HelpMessage();

  void Run(int argc, char **argv);

  static FuncFactory &instance();

protected:
  bool Run(std::string_view n);

protected:
  std::unordered_map<std::string, Func> data_;
};

} // namespace tests

#define FUNC_FACTORY_REGISTER(name, func)                                      \
  namespace {                                                                  \
  struct TestsFuncHolder {                                                     \
    TestsFuncHolder() {                                                        \
      tests::FuncFactory::instance().Register((name), (func));                 \
    }                                                                          \
  };                                                                           \
  static auto CONCAT(local_tests_func_holder, __LINE__) = TestsFuncHolder();   \
  }