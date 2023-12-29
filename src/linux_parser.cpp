#include <dirent.h>
#include <unistd.h>
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

// TODO: Update this to use std::filesystem
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
  string mem_total;
  string mem_free;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream1(line);
    linestream1 >> key >> mem_total;

    std::getline(filestream, line);
    std::istringstream linestream2(line);
    linestream2 >> key >> mem_free;

    return 100 * (std::stof(mem_total) - std::stof(mem_free)) /
           std::stof(mem_total);
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

// TODO: Read and return the number of jiffies for the system
// TODO: Use System Active jiffies + System IdleJiffies which take values from
// CpuUtilization
long LinuxParser::Jiffies() {
  return LinuxParser::UpTime() * sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  // TODO check when compared to ActiveJiffies()
  // https://knowledge.udacity.com/questions/129844
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

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  // Could use CpuUtilization here
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

// TODO: Read and return the number of idle jiffies for the system
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

// TODO: Read and return CPU utilization
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

// TODO: Read and return the total number of processes
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

// TODO: Read and return the number of running processes
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

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
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

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line) && key != "VmSize:") {
      std::istringstream linestream(line);
      linestream >> key >> value;
    };
    return value;
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
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

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
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

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid_) {
  string line, pid, comm, state, ppid, pgrp, session, tty_nr, tpgid, flags,
      minflt, cminflt, majflt, cmajflt, utime;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid_) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
        tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime;
    return std::stol(utime) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}

float LinuxParser::ProcessUtilization(int pid) {
  static float prevUserTime = 0;
  static float prevKernelTime = 0;
  static float prevWfcUserTime = 0;
  static float prevWfcKernelTime = 0;
  static float prevStartTime = 0;

  float userTime;
  float kernelTime;
  float wfcUserTime;
  float wfcKernelTime;
  float startTime;

  float totalTime;

  float elapsedTime;

  string line, key, value;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) +
                           kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);

      for (int i = 0; i < 25; i++) {
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

    totalTime = ((userTime - prevUserTime) + (kernelTime - prevKernelTime) +
                 (wfcUserTime - prevWfcUserTime) +
                 (wfcKernelTime - prevWfcKernelTime)) /
                sysconf(_SC_CLK_TCK);
    elapsedTime = LinuxParser::UpTime() -
                  ((startTime - prevStartTime) / sysconf(_SC_CLK_TCK));

    prevUserTime = userTime;
    prevKernelTime = kernelTime;
    prevWfcUserTime = wfcUserTime;
    prevWfcKernelTime = wfcKernelTime;
    prevStartTime = startTime;
  }
  return (totalTime / elapsedTime);
}