#include <chrono>
#include <thread>
#include "linux_parser.h"

#include "processor.h"

// DONE: Return the aggregate CPU utilization
// TODO: Could use static here and base of the last call.
float Processor::Utilization() {
  // Based on:
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

  static long prev_idle = LinuxParser::IdleJiffies();
  static long prev_active = LinuxParser::ActiveJiffies();
  long prev_total = prev_idle + prev_active;

  long idle = LinuxParser::IdleJiffies();
  long active = LinuxParser::ActiveJiffies();
  long total = idle + active;
  
  long total_d = total - prev_total;

  if (total_d == 0) {
    return 0.0;
  } else {
    long idle_d = idle - prev_idle;
    return static_cast<double>(total_d - idle_d) / total_d;
  }

  prev_idle = idle;
  prev_active = active;
  
}