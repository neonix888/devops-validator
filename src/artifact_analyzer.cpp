#include "artifact_analyzer.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <sstream>
#include <array>
#include <memory>

// Platform-specific popen/pclose
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

namespace fs = std::filesystem;

namespace devops {

ArtifactInfo ArtifactAnalyzer::analyzeFile(const std::string& filePath) {
    ArtifactInfo info;
    info.valid = false;

    if (!Utils::fileExists(filePath)) {
        Utils::printError("File not found: " + filePath);
        return info;
    }

    std::string ext = Utils::getFileExtension(filePath);
    info.name = fs::path(filePath).filename().string();

    try {
        info.size = formatSize(fs::file_size(filePath));
    } catch (const std::exception& e) {
        info.size = "unknown";
    }

    if (ext == ".deb") {
        info = analyzeDeb(filePath);
    } else if (ext == ".rpm") {
        info = analyzeRpm(filePath);
    } else if (filePath.find("Dockerfile") != std::string::npos) {
        info = analyzeDocker(filePath);
    } else if (ext == ".tar" || ext == ".gz" || ext == ".zip" || ext == ".tgz") {
        info = analyzeArchive(filePath);
    } else {
        info.type = "Unknown";
        info.valid = true;
    }

    printArtifactInfo(info);
    return info;
}

void ArtifactAnalyzer::analyzeDirectory(const std::string& dirPath) {
    Utils::printInfo("Analyzing artifacts in: " + dirPath);

    int filesAnalyzed = 0;

    try {
        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                std::string path = entry.path().string();
                std::string ext = Utils::getFileExtension(path);

                if (ext == ".deb" || ext == ".rpm" || ext == ".tar" ||
                    ext == ".gz" || ext == ".zip" || ext == ".tgz" ||
                    path.find("Dockerfile") != std::string::npos) {

                    std::cout << "\n" << Color::BOLD << "=== " << path << " ===" << Color::RESET << std::endl;
                    analyzeFile(path);
                    filesAnalyzed++;
                }
            }
        }
    } catch (const std::exception& e) {
        Utils::printError(std::string("Directory scan error: ") + e.what());
    }

    std::cout << "\n" << Color::BOLD << "Total artifacts analyzed: " << filesAnalyzed << Color::RESET << std::endl;
}

ArtifactInfo ArtifactAnalyzer::analyzeDeb(const std::string& filePath) {
    ArtifactInfo info;
    info.type = "DEB Package";
    info.name = fs::path(filePath).filename().string();

    try {
        info.size = formatSize(fs::file_size(filePath));
    } catch (...) {
        info.size = "unknown";
    }

    // Try to get package info using dpkg-deb
    std::string cmd = "dpkg-deb -I \"" + filePath + "\" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");

    if (pipe) {
        std::array<char, 128> buffer;
        std::string result;

        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }

        pclose(pipe);

        // Parse the output
        std::istringstream stream(result);
        std::string line;
        while (std::getline(stream, line)) {
            if (line.find("Package:") != std::string::npos) {
                info.metadata["Package"] = line.substr(line.find(":") + 2);
            } else if (line.find("Version:") != std::string::npos) {
                info.metadata["Version"] = line.substr(line.find(":") + 2);
            } else if (line.find("Architecture:") != std::string::npos) {
                info.metadata["Architecture"] = line.substr(line.find(":") + 2);
            } else if (line.find("Depends:") != std::string::npos) {
                std::string deps = line.substr(line.find(":") + 2);
                info.dependencies = Utils::split(deps, ',');
            }
        }

        info.valid = true;
    } else {
        info.metadata["Note"] = "dpkg-deb not available - limited analysis";
        info.valid = true;
    }

    return info;
}

ArtifactInfo ArtifactAnalyzer::analyzeRpm(const std::string& filePath) {
    ArtifactInfo info;
    info.type = "RPM Package";
    info.name = fs::path(filePath).filename().string();

    try {
        info.size = formatSize(fs::file_size(filePath));
    } catch (...) {
        info.size = "unknown";
    }

    // Try to get package info using rpm
    std::string cmd = "rpm -qip \"" + filePath + "\" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");

    if (pipe) {
        std::array<char, 128> buffer;
        std::string result;

        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }

        pclose(pipe);

        // Parse the output
        std::istringstream stream(result);
        std::string line;
        while (std::getline(stream, line)) {
            if (line.find("Name") != std::string::npos) {
                info.metadata["Name"] = line.substr(line.find(":") + 2);
            } else if (line.find("Version") != std::string::npos) {
                info.metadata["Version"] = line.substr(line.find(":") + 2);
            } else if (line.find("Architecture") != std::string::npos) {
                info.metadata["Architecture"] = line.substr(line.find(":") + 2);
            }
        }

        info.valid = true;
    } else {
        info.metadata["Note"] = "rpm command not available - limited analysis";
        info.valid = true;
    }

    return info;
}

ArtifactInfo ArtifactAnalyzer::analyzeDocker(const std::string& filePath) {
    ArtifactInfo info;
    info.type = "Dockerfile";
    info.name = fs::path(filePath).filename().string();
    info.valid = true;

    try {
        std::string content = Utils::readFile(filePath);
        std::istringstream stream(content);
        std::string line;

        int fromCount = 0;
        int runCount = 0;
        int copyCount = 0;

        while (std::getline(stream, line)) {
            if (line.find("FROM") == 0) {
                fromCount++;
                std::string baseImage = line.substr(5);
                info.dependencies.push_back("Base: " + baseImage);
            } else if (line.find("RUN") == 0) {
                runCount++;
            } else if (line.find("COPY") == 0) {
                copyCount++;
            } else if (line.find("EXPOSE") == 0) {
                info.metadata["Ports"] = line.substr(7);
            }
        }

        info.metadata["FROM Instructions"] = std::to_string(fromCount);
        info.metadata["RUN Instructions"] = std::to_string(runCount);
        info.metadata["COPY Instructions"] = std::to_string(copyCount);
        info.metadata["Multi-stage"] = (fromCount > 1) ? "Yes" : "No";

    } catch (const std::exception& e) {
        info.metadata["Error"] = e.what();
        info.valid = false;
    }

    return info;
}

ArtifactInfo ArtifactAnalyzer::analyzeArchive(const std::string& filePath) {
    ArtifactInfo info;
    info.type = "Archive";
    info.name = fs::path(filePath).filename().string();

    try {
        info.size = formatSize(fs::file_size(filePath));
    } catch (...) {
        info.size = "unknown";
    }

    std::string ext = Utils::getFileExtension(filePath);
    info.metadata["Format"] = ext;

    // Try to list contents
    std::string cmd;
    if (ext == ".tar" || ext == ".tgz" || ext == ".gz") {
        cmd = "tar -tzf \"" + filePath + "\" 2>/dev/null | wc -l";
    } else if (ext == ".zip") {
        cmd = "unzip -l \"" + filePath + "\" 2>/dev/null | tail -1";
    }

    if (!cmd.empty()) {
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                info.metadata["Files"] = std::string(buffer);
            }
            pclose(pipe);
        }
    }

    info.valid = true;
    return info;
}

void ArtifactAnalyzer::printArtifactInfo(const ArtifactInfo& info) {
    std::cout << Color::BOLD << "Type: " << Color::RESET << info.type << std::endl;
    std::cout << Color::BOLD << "Name: " << Color::RESET << info.name << std::endl;

    if (!info.size.empty()) {
        std::cout << Color::BOLD << "Size: " << Color::RESET << info.size << std::endl;
    }

    if (!info.metadata.empty()) {
        std::cout << Color::BOLD << "Metadata:" << Color::RESET << std::endl;
        for (const auto& [key, value] : info.metadata) {
            std::cout << "  " << Color::CYAN << key << ": " << Color::RESET << value << std::endl;
        }
    }

    if (!info.dependencies.empty()) {
        std::cout << Color::BOLD << "Dependencies:" << Color::RESET << std::endl;
        for (const auto& dep : info.dependencies) {
            std::cout << "  - " << dep << std::endl;
        }
    }

    if (info.valid) {
        Utils::printSuccess("Artifact analysis complete");
    } else {
        Utils::printWarning("Artifact analysis incomplete");
    }
}

std::string ArtifactAnalyzer::formatSize(long bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f %s", size, units[unitIndex]);
    return std::string(buffer);
}

} // namespace devops
