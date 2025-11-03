#pragma once

#include <map>
#include <string>
#include <vector>

namespace devops {

struct HealthCheckResult {
  bool healthy;
  std::vector<std::string> issues;
  std::vector<std::string> warnings;
  std::map<std::string, std::string> systemInfo;
};

class HealthChecker {
public:
  HealthCheckResult checkSystem();
  HealthCheckResult checkTools();
  HealthCheckResult checkEnvironment();

  void printReport();

private:
  bool checkCommand(const std::string &command);
  std::string getCommandVersion(const std::string &command);
  std::string getOSInfo();
  std::string getCPUInfo();
  std::string getMemoryInfo();
  std::string getDiskInfo();
};

} // namespace devops
