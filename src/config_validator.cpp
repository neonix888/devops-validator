#include "config_validator.h"
#include "utils.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace devops {

ValidationResult ConfigValidator::validateFile(const std::string &filePath) {
  ValidationResult result;
  result.valid = false;

  if (!Utils::fileExists(filePath)) {
    result.errors.push_back("File not found: " + filePath);
    return result;
  }

  std::string content;
  try {
    content = Utils::readFile(filePath);
  } catch (const std::exception &e) {
    result.errors.push_back(std::string("Failed to read file: ") + e.what());
    return result;
  }

  std::string ext = Utils::getFileExtension(filePath);

  if (ext == ".json") {
    result = validateJSON(content, filePath);
  } else if (ext == ".yaml" || ext == ".yml") {
    result = validateYAML(content, filePath);
  } else if (ext == ".toml") {
    result = validateTOML(content, filePath);
  } else if (ext == ".env" || filePath.find(".env") != std::string::npos) {
    result = validateEnv(content, filePath);
  } else {
    result.warnings.push_back("Unknown file type, attempting JSON parse");
    result = validateJSON(content, filePath);
  }

  printValidationResult(result, filePath);
  return result;
}

ValidationResult
ConfigValidator::validateDirectory(const std::string &dirPath) {
  ValidationResult overallResult;
  overallResult.valid = true;
  overallResult.fileType = "directory";

  Utils::printInfo("Scanning directory: " + dirPath);

  int filesChecked = 0;
  int filesValid = 0;

  try {
    for (const auto &entry : fs::recursive_directory_iterator(dirPath)) {
      if (entry.is_regular_file()) {
        std::string path = entry.path().string();
        std::string ext = Utils::getFileExtension(path);

        if (ext == ".json" || ext == ".yaml" || ext == ".yml" ||
            ext == ".toml" || ext == ".env" ||
            path.find(".env") != std::string::npos) {

          std::cout << "\n"
                    << Color::BOLD << "Validating: " << path << Color::RESET
                    << std::endl;
          ValidationResult result = validateFile(path);
          filesChecked++;

          if (result.valid) {
            filesValid++;
          } else {
            overallResult.valid = false;
            overallResult.errors.insert(overallResult.errors.end(),
                                        result.errors.begin(),
                                        result.errors.end());
          }

          overallResult.warnings.insert(overallResult.warnings.end(),
                                        result.warnings.begin(),
                                        result.warnings.end());
        }
      }
    }
  } catch (const std::exception &e) {
    overallResult.errors.push_back(std::string("Directory scan error: ") +
                                   e.what());
    overallResult.valid = false;
  }

  std::cout << "\n"
            << Color::BOLD
            << "=== Directory Validation Summary ===" << Color::RESET
            << std::endl;
  std::cout << "Files checked: " << filesChecked << std::endl;
  std::cout << "Files valid: " << filesValid << std::endl;
  std::cout << "Files invalid: " << (filesChecked - filesValid) << std::endl;

  return overallResult;
}

ValidationResult ConfigValidator::validateJSON(const std::string &content,
                                               const std::string &filePath) {
  ValidationResult result;
  result.fileType = "JSON";

  try {
    json j = json::parse(content);
    result.valid = true;

    // Additional checks
    if (j.is_object() && j.empty()) {
      result.warnings.push_back("JSON object is empty");
    }

    // Check for common DevOps config patterns
    if (j.contains("version") && j["version"].is_string()) {
      Utils::printInfo("Version: " + j["version"].get<std::string>());
    }

  } catch (const json::parse_error &e) {
    result.valid = false;
    result.errors.push_back("JSON parse error at byte " +
                            std::to_string(e.byte) + ": " + e.what());
  }

  return result;
}

ValidationResult ConfigValidator::validateYAML(const std::string &content,
                                               const std::string &filePath) {
  ValidationResult result;
  result.fileType = "YAML";

  try {
    YAML::Node config = YAML::Load(content);
    result.valid = true;

    if (config.IsNull()) {
      result.warnings.push_back("YAML file is empty");
    }

    // Check for Ansible playbook
    if (config.IsSequence() && config.size() > 0 && config[0]["hosts"]) {
      Utils::printInfo("Detected Ansible playbook");
    }

    // Check for Docker Compose
    if (config["services"]) {
      Utils::printInfo("Detected Docker Compose file");
      if (!config["version"]) {
        result.warnings.push_back("Docker Compose 'version' field missing");
      }
    }

    // Check for Kubernetes
    if (config["apiVersion"] && config["kind"]) {
      Utils::printInfo("Detected Kubernetes manifest");
    }

  } catch (const YAML::Exception &e) {
    result.valid = false;
    result.errors.push_back(std::string("YAML parse error: ") + e.what());
  }

  return result;
}

ValidationResult ConfigValidator::validateTOML(const std::string &content,
                                               const std::string &filePath) {
  ValidationResult result;
  result.fileType = "TOML";

  // Basic TOML validation (regex-based since we don't have a TOML library)
  // Check for basic TOML syntax
  std::regex sectionRegex(R"(\[[\w\.\-]+\])");
  std::regex keyValueRegex(R"([\w\-]+\s*=\s*.+)");

  bool hasContent = false;
  std::istringstream stream(content);
  std::string line;
  int lineNum = 0;

  while (std::getline(stream, line)) {
    lineNum++;
    // Skip comments and empty lines
    std::string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));

    if (trimmed.empty() || trimmed[0] == '#') {
      continue;
    }

    hasContent = true;

    // Check if it's a section or key-value pair
    if (!std::regex_search(trimmed, sectionRegex) &&
        !std::regex_search(trimmed, keyValueRegex)) {
      result.warnings.push_back("Line " + std::to_string(lineNum) +
                                " doesn't match TOML syntax: " + trimmed);
    }
  }

  if (!hasContent) {
    result.warnings.push_back("TOML file appears to be empty");
  }

  result.valid = true; // We're lenient with TOML
  return result;
}

ValidationResult ConfigValidator::validateEnv(const std::string &content,
                                              const std::string &filePath) {
  ValidationResult result;
  result.fileType = "ENV";

  std::regex envRegex(R"(^[\w]+=[^\s]*)");
  std::regex commentRegex(R"(^\s*#)");

  std::istringstream stream(content);
  std::string line;
  int lineNum = 0;
  int validVars = 0;

  while (std::getline(stream, line)) {
    lineNum++;

    if (line.empty() || std::regex_search(line, commentRegex)) {
      continue;
    }

    if (std::regex_search(line, envRegex)) {
      validVars++;

      // Check for unquoted values with spaces
      if (line.find('=') != std::string::npos) {
        std::string value = line.substr(line.find('=') + 1);
        if (value.find(' ') != std::string::npos && value[0] != '"' &&
            value[0] != '\'') {
          result.warnings.push_back("Line " + std::to_string(lineNum) +
                                    ": unquoted value with spaces");
        }
      }
    } else {
      result.warnings.push_back("Line " + std::to_string(lineNum) +
                                " doesn't match ENV syntax: " + line);
    }
  }

  if (validVars == 0) {
    result.warnings.push_back("No valid environment variables found");
  }

  result.valid = true;
  Utils::printInfo("Found " + std::to_string(validVars) +
                   " environment variables");

  return result;
}

void ConfigValidator::printValidationResult(const ValidationResult &result,
                                            const std::string &filePath) {
  if (result.valid) {
    Utils::printSuccess("Valid " + result.fileType + " file");
  } else {
    Utils::printError("Invalid " + result.fileType + " file");
  }

  for (const auto &error : result.errors) {
    std::cerr << Color::RED << "  ERROR: " << error << Color::RESET
              << std::endl;
  }

  for (const auto &warning : result.warnings) {
    std::cout << Color::YELLOW << "  WARNING: " << warning << Color::RESET
              << std::endl;
  }
}

} // namespace devops
