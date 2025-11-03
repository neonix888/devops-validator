#pragma once

#include <string>
#include <vector>

namespace devops {

// ANSI color codes for terminal output
namespace Color {
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string BOLD = "\033[1m";
} // namespace Color

// Utility functions
class Utils {
public:
  static bool fileExists(const std::string &path);
  static std::string readFile(const std::string &path);
  static bool hasExtension(const std::string &path, const std::string &ext);
  static std::vector<std::string> split(const std::string &str, char delimiter);
  static void printSuccess(const std::string &message);
  static void printError(const std::string &message);
  static void printWarning(const std::string &message);
  static void printInfo(const std::string &message);
  static std::string getFileExtension(const std::string &path);
};

} // namespace devops
