#include "bench/bench.h"

int main(int argc, char **argv) {
  utils::ShowBuildInfo(std::cout);
  MSGLN("------");
  bench::FuncFactory::instance().Run(argc - 1, &argv[1]);
  return 0;
}