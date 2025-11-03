#pragma once

#include <string>
#include <vector>
#include <map>

namespace devops {

struct ArtifactInfo {
    std::string name;
    std::string type;
    std::string size;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> metadata;
    bool valid;
};

class ArtifactAnalyzer {
public:
    ArtifactInfo analyzeFile(const std::string& filePath);
    void analyzeDirectory(const std::string& dirPath);

private:
    ArtifactInfo analyzeDeb(const std::string& filePath);
    ArtifactInfo analyzeRpm(const std::string& filePath);
    ArtifactInfo analyzeDocker(const std::string& filePath);
    ArtifactInfo analyzeArchive(const std::string& filePath);

    void printArtifactInfo(const ArtifactInfo& info);
    std::string formatSize(long bytes);
};

} // namespace devops
