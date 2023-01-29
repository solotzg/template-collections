#include "work.h"
#include <chrono>

TimeCost::TimeCost(const char *label) : label_(label), start_(Clock::now()) {}

void TimeCost::Show(const char *prefix) const {
  auto end = Clock::now();

  FMSG("[%s]", label_);
  if (prefix)
    FMSG("[%s]", prefix);
  FMSGLN("[time cost: %ldms]",
         std::chrono::duration_cast<std::chrono::milliseconds>(end - start_)
             .count());
}

TimeCost::~TimeCost() { Show("END"); }

void TimeCost::Reset() { start_ = Clock::now(); }

TimeCost::Clock::duration TimeCost::Duration() { return Clock::now() - start_; }
