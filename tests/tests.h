#pragma once

#include <utils/utils.h>

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

namespace {

template <typename T>
struct TestNode : utils::ConsistencyChecker<>, utils::noncopyable {
  template <typename... Args>
  TestNode(Args &&...args) : value_(std::forward<Args>(args)...) {}
  TestNode(TestNode &&src) : value_(std::move(src.value_)) {}
  TestNode &operator=(TestNode &&src) {
    value_ = std::move(src.value_);
    return *this;
  }
  T &value() { return value_; }
  const T &value() const { return value_; }

private:
  T value_;
};
} // namespace

#define FUNC_FACTORY_REGISTER(name, func)                                      \
  namespace {                                                                  \
  struct TestsFuncHolder {                                                     \
    TestsFuncHolder() {                                                        \
      tests::FuncFactory::instance().Register((name), (func));                 \
    }                                                                          \
  };                                                                           \
  static auto CONCAT(local_tests_func_holder, __LINE__) = TestsFuncHolder();   \
  }