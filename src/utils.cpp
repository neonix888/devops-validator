#include "utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>

namespace devops {

bool Utils::fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string Utils::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Utils::hasExtension(const std::string& path, const std::string& ext) {
    if (path.length() < ext.length()) {
        return false;
    }
    return path.compare(path.length() - ext.length(), ext.length(), ext) == 0;
}

std::vector<std::string> Utils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

void Utils::printSuccess(const std::string& message) {
    std::cout << Color::GREEN << "✓ " << message << Color::RESET << std::endl;
}

void Utils::printError(const std::string& message) {
    std::cerr << Color::RED << "✗ " << message << Color::RESET << std::endl;
}

void Utils::printWarning(const std::string& message) {
    std::cout << Color::YELLOW << "⚠ " << message << Color::RESET << std::endl;
}

void Utils::printInfo(const std::string& message) {
    std::cout << Color::CYAN << "ℹ " << message << Color::RESET << std::endl;
}

std::string Utils::getFileExtension(const std::string& path) {
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    return path.substr(dotPos);
}

} // namespace devops
