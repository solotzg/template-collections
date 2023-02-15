#include "work.h"

TimeCost::TimeCost(const char *label) : label_(label), start_(Clock::now()) {}

void TimeCost::Show(const char *prefix) const {
  auto end = Clock::now();

  FMSGLN("[{}]{}[time cost: {}]", label_,
         prefix ? fmt::format("[{}]", prefix) : "",
         std::chrono::duration_cast<std::chrono::milliseconds>(end - start_));
}

TimeCost::~TimeCost() { Show("END"); }

void TimeCost::Reset() { start_ = Clock::now(); }

TimeCost::Clock::duration TimeCost::Duration() { return Clock::now() - start_; }
