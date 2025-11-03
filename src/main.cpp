#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "config_validator.h"
#include "artifact_analyzer.h"
#include "health_checker.h"
#include "utils.h"

void printBanner() {
    std::cout << devops::Color::BOLD << devops::Color::CYAN << R"(
╔══════════════════════════════════════════════════════════════╗
║                    DEVOPS VALIDATOR                          ║
║              Multi-Platform DevOps CLI Tool                  ║
║                     Version 1.0.0                            ║
╚══════════════════════════════════════════════════════════════╝
)" << devops::Color::RESET << std::endl;
}

void printUsage(const char* programName) {
    std::cout << devops::Color::BOLD << "Usage:" << devops::Color::RESET << std::endl;
    std::cout << "  " << programName << " <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << devops::Color::BOLD << "Commands:" << devops::Color::RESET << std::endl;
    std::cout << "  " << devops::Color::GREEN << "validate" << devops::Color::RESET
              << " <file|dir>    Validate configuration files (JSON/YAML/TOML/ENV)" << std::endl;
    std::cout << "  " << devops::Color::GREEN << "analyze" << devops::Color::RESET
              << "  <file|dir>    Analyze build artifacts (DEB/RPM/Docker/Archives)" << std::endl;
    std::cout << "  " << devops::Color::GREEN << "health" << devops::Color::RESET
              << "              Check system and DevOps tools health" << std::endl;
    std::cout << "  " << devops::Color::GREEN << "version" << devops::Color::RESET
              << "             Show version information" << std::endl;
    std::cout << "  " << devops::Color::GREEN << "help" << devops::Color::RESET
              << "                Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << devops::Color::BOLD << "Examples:" << devops::Color::RESET << std::endl;
    std::cout << "  " << programName << " validate config.json" << std::endl;
    std::cout << "  " << programName << " validate /path/to/configs/" << std::endl;
    std::cout << "  " << programName << " analyze build.deb" << std::endl;
    std::cout << "  " << programName << " analyze /path/to/artifacts/" << std::endl;
    std::cout << "  " << programName << " health" << std::endl;
    std::cout << std::endl;
}

void printVersion() {
    std::cout << "DevOps Validator v1.0.0" << std::endl;
    std::cout << "Built with CMake for multi-platform deployment" << std::endl;
    std::cout << "Supports: Linux, macOS, Windows" << std::endl;
    std::cout << "Package formats: DEB, RPM, MSI, Homebrew, pip, npm" << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();

    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string command = argv[1];

    if (command == "help" || command == "--help" || command == "-h") {
        printUsage(argv[0]);
        return 0;
    }

    if (command == "version" || command == "--version" || command == "-v") {
        printVersion();
        return 0;
    }

    if (command == "validate") {
        if (argc < 3) {
            devops::Utils::printError("Missing file or directory argument");
            std::cout << "Usage: " << argv[0] << " validate <file|dir>" << std::endl;
            return 1;
        }

        std::string target = argv[2];
        devops::ConfigValidator validator;

        try {
            if (std::filesystem::is_directory(target)) {
                auto result = validator.validateDirectory(target);
                return result.valid ? 0 : 1;
            } else {
                auto result = validator.validateFile(target);
                return result.valid ? 0 : 1;
            }
        } catch (const std::exception& e) {
            devops::Utils::printError(std::string("Validation failed: ") + e.what());
            return 1;
        }
    }

    if (command == "analyze") {
        if (argc < 3) {
            devops::Utils::printError("Missing file or directory argument");
            std::cout << "Usage: " << argv[0] << " analyze <file|dir>" << std::endl;
            return 1;
        }

        std::string target = argv[2];
        devops::ArtifactAnalyzer analyzer;

        try {
            if (std::filesystem::is_directory(target)) {
                analyzer.analyzeDirectory(target);
            } else {
                analyzer.analyzeFile(target);
            }
            return 0;
        } catch (const std::exception& e) {
            devops::Utils::printError(std::string("Analysis failed: ") + e.what());
            return 1;
        }
    }

    if (command == "health") {
        try {
            devops::HealthChecker checker;
            checker.printReport();
            return 0;
        } catch (const std::exception& e) {
            devops::Utils::printError(std::string("Health check failed: ") + e.what());
            return 1;
        }
    }

    devops::Utils::printError("Unknown command: " + command);
    printUsage(argv[0]);
    return 1;
}
