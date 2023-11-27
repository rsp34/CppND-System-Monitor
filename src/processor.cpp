#include <thread>
#include <chrono>
#include "linux_parser.h"

#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
     // Based on: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

  long prev_idle = LinuxParser::IdleJiffies();
  long prev_active = LinuxParser::ActiveJiffies();
  long prev_total = prev_idle + prev_active;

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  long idle = LinuxParser::IdleJiffies();
  long active = LinuxParser::ActiveJiffies();
  long total = idle + active;

  long total_d = total - prev_total;
  long idle_d = idle - prev_idle;

  return 100*((total_d-idle_d)/total_d);
  
  }