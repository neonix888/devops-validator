#pragma once

#include <string>
#include <vector>

namespace devops {

struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::string fileType;
};

class ConfigValidator {
public:
    ValidationResult validateFile(const std::string& filePath);
    ValidationResult validateDirectory(const std::string& dirPath);

private:
    ValidationResult validateJSON(const std::string& content, const std::string& filePath);
    ValidationResult validateYAML(const std::string& content, const std::string& filePath);
    ValidationResult validateTOML(const std::string& content, const std::string& filePath);
    ValidationResult validateEnv(const std::string& content, const std::string& filePath);

    void printValidationResult(const ValidationResult& result, const std::string& filePath);
};

} // namespace devops
