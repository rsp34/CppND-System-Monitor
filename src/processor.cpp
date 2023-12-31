#include <chrono>
#include <thread>
#include "linux_parser.h"

#include "processor.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  // Based on:
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

  static long prevIdle = LinuxParser::IdleJiffies();
  static long prevActive = LinuxParser::ActiveJiffies();
  long prevTotal = prevIdle + prevActive;

  long idle = LinuxParser::IdleJiffies();
  long active = LinuxParser::ActiveJiffies();
  long total = idle + active;

  long totalD = total - prevTotal;

  if (totalD == 0) {
    return 0.0;
  } else {
    long idleD = idle - prevIdle;
    return static_cast<double>(totalD - idleD) / totalD;
  }

  prevIdle = idle;
  prevActive = active;
}