#include <chrono>
#include <iostream>

#include "spdlog/spdlog.h"

template <class Logger>
void run_test(Logger& logger, const char* name) {
  using Clock = std::chrono::high_resolution_clock;

  auto start = Clock::now();

  for (int i=0; i<1000000; ++i) {
    logger->info("Hello from test");
  }
  auto stop = Clock::now();

  std::cout << "Test:" << name << " time:" << (stop - start).count();
  std::cout << std::endl;
}

int main() {
  std::vector<spdlog::sink_ptr> logger_with_switch_sinks;
  logger_with_switch_sinks.push_back(
      std::make_shared<spdlog::sinks::android_switch_sink>()
  );
  auto logger_with_switch = std::make_shared<spdlog::logger>(
      "1", begin(logger_with_switch_sinks), end(logger_with_switch_sinks)
  );

  std::vector<spdlog::sink_ptr> logger_with_array_sinks;
  logger_with_array_sinks.push_back(
      std::make_shared<spdlog::sinks::android_array_sink>()
  );
  auto logger_with_array = std::make_shared<spdlog::logger>(
      "2", begin(logger_with_array_sinks), end(logger_with_array_sinks)
  );

  run_test(logger_with_switch, "switch");
  run_test(logger_with_array, "array");

  run_test(logger_with_switch, "switch");
  run_test(logger_with_array, "array");

  run_test(logger_with_switch, "switch");
  run_test(logger_with_array, "array");
}
