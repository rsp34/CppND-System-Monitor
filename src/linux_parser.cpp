#include <dirent.h>
#include <unistd.h>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// DONE: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string memTotal;
  string memFree;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream1(line);
    linestream1 >> key >> memTotal;

    std::getline(filestream, line);
    std::istringstream linestream2(line);
    linestream2 >> key >> memFree;

    return (std::stof(memTotal) - std::stof(memFree)) / std::stof(memTotal);
  }
  return 0;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  std::string line;
  std::string uptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return std::stol(uptime);
  };
  return 0;
}

// DONE: Read and return the number of jiffies for the system
// TODO: Compare against System Active jiffies + System IdleJiffies which take
// values from
// CpuUtilization
long LinuxParser::Jiffies() {
  return LinuxParser::UpTime() * sysconf(_SC_CLK_TCK);
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string active_jiffies;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i <= 21; i++) {
      linestream >> active_jiffies;
    };
    return std::stol(active_jiffies);
  };
  return 0;
}

// DONE: Read and return the number of active jiffies for the system
// TODO: Use CpuUtilization here and CpuStates
long LinuxParser::ActiveJiffies() {
  string line;
  string cpu, user, nice, system, idle, iowait, irq, softirq, steal;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal;
    return std::stol(user) + std::stol(nice) + std::stol(system) +
           std::stol(irq) + std::stol(softirq) + std::stol(steal);
  }
  return 0;
}

// DONE: Read and return the number of idle jiffies for the system
// TODO: Use CpuUtilization here and CpuStates
long LinuxParser::IdleJiffies() {
  string line;
  string cpu, user, nice, system, idle, iowait;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait;
    return std::stol(idle) + std::stol(iowait);
  }
  return 0;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string cpu, user, nice, system, idle, iowait, irq, softirq, steal, guest,
      guest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;
    return {user, nice,    system, idle,  iowait,
            irq,  softirq, steal,  guest, guest_nice};
  }
  return {};
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line) && key != "processes") {
      std::istringstream linestream(line);
      linestream >> key >> value;
    };
    return std::stoi(value);
  }
  return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line) && key != "procs_running") {
      std::istringstream linestream(line);
      linestream >> key >> value;
    };
    return std::stoi(value);
  }
  return 0;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return string();
}

// DONE: Read and return the memory used by a process
// TODO: Limited precision here - character width might have been more sensible.
string LinuxParser::Ram(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line) && key != "VmSize:") {
      std::istringstream linestream(line);
      linestream >> key >> value;
    };
    std::stringstream stream;
    stream << std::fixed << std::setprecision(1) << stof(value) / 1024;
    return stream.str();
  }
  return string();
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line) && key != "Uid:") {
      std::istringstream linestream(line);
      linestream >> key >> value;
    };
    return value;
  }
  return string();
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, username, password, uid;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> username >> password >> uid;
      if (uid == Uid(pid)) {
        return username;
      };
    };
  }
  return string();
}

// DONE: Read and return the uptime of a process
// TODO: Could refactor any functionality shared with
// LinuxParser::ProcessUtilisation.
long LinuxParser::UpTime(int pid_) {
  string value;
  long starttime;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid_) +
                           kStatFilename);
  if (filestream.is_open()) {
    for (int i = 1; i <= 22; i++) filestream >> value;
    starttime = std::stol(value);
    filestream.close();
  }

  long uptime = LinuxParser::UpTime();
  long process_uptime = uptime - (starttime / sysconf(_SC_CLK_TCK));
  return process_uptime;
}

// TODO: Could refactor any functionality shared with LinuxParser::UpTime(int
// pid_).
float LinuxParser::ProcessUtilization(int pid) {
  // Based on:
  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599

  float userTime, kernelTime, wfcUserTime, wfcKernelTime, startTime, totalTime,
      elapsedTime;
  string line, key, value;

  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);

      for (int i = 0; i <= 21; i++) {
        linestream >> value;
        if (i == 13)
          userTime = stof(value);
        else if (i == 14)
          kernelTime = stof(value);
        else if (i == 15)
          wfcUserTime = stof(value);
        else if (i == 16)
          wfcKernelTime = stof(value);
        else if (i == 21)
          startTime = stof(value);
      }
    }

    totalTime = (userTime + kernelTime + wfcUserTime + wfcKernelTime) /
                sysconf(_SC_CLK_TCK);
    elapsedTime = LinuxParser::UpTime() - startTime / sysconf(_SC_CLK_TCK);
  }
  return totalTime / elapsedTime;
}