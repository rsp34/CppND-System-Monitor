#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
//Solution https://knowledge.udacity.com/questions/834579
float Process::CpuUtilization() const { return 0; }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Process::Pid()); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Process::Pid()); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Process::Pid()); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Process::Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
// Rank by CPU utilisation
bool Process::operator<(Process const& a) const {
    if(Process::CpuUtilization() < a.CpuUtilization()){
        return true;
    }
    return false;