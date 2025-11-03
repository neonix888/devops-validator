#include "health_checker.h"
#include "utils.h"
#include <iostream>
#include <array>
#include <cstdlib>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#define popen _popen
#define pclose _pclose
#else
#include <sys/utsname.h>
#include <unistd.h>
#endif

namespace devops {

HealthCheckResult HealthChecker::checkSystem() {
    HealthCheckResult result;
    result.healthy = true;

    Utils::printInfo("Checking system information...");

    result.systemInfo["OS"] = getOSInfo();
    result.systemInfo["CPU"] = getCPUInfo();
    result.systemInfo["Memory"] = getMemoryInfo();
    result.systemInfo["Disk"] = getDiskInfo();

    return result;
}

HealthCheckResult HealthChecker::checkTools() {
    HealthCheckResult result;
    result.healthy = true;

    Utils::printInfo("Checking DevOps tools...");

    // Essential DevOps tools
    std::vector<std::string> tools = {
        "git", "docker", "kubectl", "ansible", "terraform",
        "cmake", "make", "gcc", "python3", "node", "npm"
    };

    for (const auto& tool : tools) {
        if (checkCommand(tool)) {
            std::string version = getCommandVersion(tool);
            result.systemInfo[tool] = version;
            Utils::printSuccess(tool + ": " + version);
        } else {
            result.warnings.push_back(tool + " not found");
            Utils::printWarning(tool + " not found");
        }
    }

    return result;
}

HealthCheckResult HealthChecker::checkEnvironment() {
    HealthCheckResult result;
    result.healthy = true;

    Utils::printInfo("Checking environment variables...");

    // Check important environment variables
    std::vector<std::string> envVars = {
        "PATH", "HOME", "USER", "SHELL",
        "CI", "GITHUB_ACTIONS", "DOCKER_HOST"
    };

    for (const auto& var : envVars) {
        const char* value = std::getenv(var.c_str());
        if (value) {
            result.systemInfo[var] = value;
            Utils::printSuccess(var + " is set");
        } else {
            if (var == "CI" || var == "GITHUB_ACTIONS" || var == "DOCKER_HOST") {
                // These are optional
                result.systemInfo[var] = "(not set)";
            } else {
                result.warnings.push_back(var + " not set");
            }
        }
    }

    return result;
}

void HealthChecker::printReport() {
    std::cout << "\n" << Color::BOLD << Color::CYAN
              << "==================================================" << std::endl
              << "         DEVOPS SYSTEM HEALTH REPORT" << std::endl
              << "==================================================" << Color::RESET << std::endl;

    auto systemResult = checkSystem();
    std::cout << "\n" << Color::BOLD << "System Information:" << Color::RESET << std::endl;
    for (const auto& [key, value] : systemResult.systemInfo) {
        std::cout << "  " << Color::CYAN << key << ": " << Color::RESET << value << std::endl;
    }

    auto toolsResult = checkTools();
    std::cout << "\n" << Color::BOLD << "DevOps Tools:" << Color::RESET << std::endl;
    // Already printed during check

    auto envResult = checkEnvironment();

    // Summary
    std::cout << "\n" << Color::BOLD << "Summary:" << Color::RESET << std::endl;

    int totalWarnings = systemResult.warnings.size() +
                       toolsResult.warnings.size() +
                       envResult.warnings.size();

    if (totalWarnings == 0) {
        Utils::printSuccess("System is healthy - all checks passed!");
    } else {
        Utils::printWarning("System has " + std::to_string(totalWarnings) + " warnings");
    }

    std::cout << Color::BOLD << Color::CYAN
              << "==================================================" << Color::RESET << std::endl;
}

bool HealthChecker::checkCommand(const std::string& command) {
#ifdef _WIN32
    std::string cmd = "where " + command + " >nul 2>&1";
#else
    std::string cmd = "command -v " + command + " >/dev/null 2>&1";
#endif

    return (system(cmd.c_str()) == 0);
}

std::string HealthChecker::getCommandVersion(const std::string& command) {
    std::string versionCmd;

    if (command == "git") {
        versionCmd = "git --version 2>&1";
    } else if (command == "docker") {
        versionCmd = "docker --version 2>&1";
    } else if (command == "kubectl") {
        versionCmd = "kubectl version --client --short 2>&1";
    } else if (command == "ansible") {
        versionCmd = "ansible --version 2>&1 | head -1";
    } else if (command == "terraform") {
        versionCmd = "terraform --version 2>&1 | head -1";
    } else if (command == "cmake") {
        versionCmd = "cmake --version 2>&1 | head -1";
    } else if (command == "make") {
        versionCmd = "make --version 2>&1 | head -1";
    } else if (command == "gcc") {
        versionCmd = "gcc --version 2>&1 | head -1";
    } else if (command == "python3") {
        versionCmd = "python3 --version 2>&1";
    } else if (command == "node") {
        versionCmd = "node --version 2>&1";
    } else if (command == "npm") {
        versionCmd = "npm --version 2>&1";
    } else {
        versionCmd = command + " --version 2>&1 | head -1";
    }

    FILE* pipe = popen(versionCmd.c_str(), "r");
    if (!pipe) {
        return "installed";
    }

    std::array<char, 256> buffer;
    std::string result;

    if (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result = buffer.data();
        // Remove newline
        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }
    }

    pclose(pipe);
    return result.empty() ? "installed" : result;
}

std::string HealthChecker::getOSInfo() {
#ifdef _WIN32
    return "Windows";
#else
    struct utsname buffer;
    if (uname(&buffer) == 0) {
        return std::string(buffer.sysname) + " " + buffer.release;
    }
    return "Unknown Unix";
#endif
}

std::string HealthChecker::getCPUInfo() {
#ifdef _WIN32
    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo);
    return std::to_string(siSysInfo.dwNumberOfProcessors) + " cores";
#else
    long numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    if (numCPU > 0) {
        return std::to_string(numCPU) + " cores";
    }
    return "Unknown";
#endif
}

std::string HealthChecker::getMemoryInfo() {
#ifdef _WIN32
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    long long totalGB = statex.ullTotalPhys / (1024 * 1024 * 1024);
    return std::to_string(totalGB) + " GB";
#else
    long pages = sysconf(_SC_PHYS_PAGES);
    long pageSize = sysconf(_SC_PAGE_SIZE);
    if (pages > 0 && pageSize > 0) {
        long long totalGB = (pages * pageSize) / (1024 * 1024 * 1024);
        return std::to_string(totalGB) + " GB";
    }
    return "Unknown";
#endif
}

std::string HealthChecker::getDiskInfo() {
#ifdef _WIN32
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    if (GetDiskFreeSpaceEx(NULL, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
        long long totalGB = totalNumberOfBytes.QuadPart / (1024 * 1024 * 1024);
        long long freeGB = totalNumberOfFreeBytes.QuadPart / (1024 * 1024 * 1024);
        return std::to_string(freeGB) + " GB free / " + std::to_string(totalGB) + " GB total";
    }
    return "Unknown";
#else
    FILE* pipe = popen("df -h / 2>/dev/null | tail -1 | awk '{print $4 \" free / \" $2 \" total\"}'", "r");
    if (pipe) {
        std::array<char, 128> buffer;
        std::string result;
        if (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result = buffer.data();
            if (!result.empty() && result.back() == '\n') {
                result.pop_back();
            }
        }
        pclose(pipe);
        return result.empty() ? "Unknown" : result;
    }
    return "Unknown";
#endif
}

} // namespace devops
