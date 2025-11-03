# DevOps Validator

[![CI/CD Pipeline](https://github.com/neonix888/devops-validator/actions/workflows/ci.yml/badge.svg)](https://github.com/neonix888/devops-validator/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-linux%20%7C%20macos%20%7C%20windows-lightgrey)](https://github.com/neonix888/devops-validator)

**A comprehensive, multi-platform DevOps validation CLI tool built with modern CMake** - demonstrating professional DevOps engineering practices through automated build pipelines, cross-platform packaging, and containerization.

## 🎯 Project Purpose

This project was created to demonstrate expertise in the exact skills required for a DevOps Engineer role:
- ✅ **CMake** for C++ project builds
- ✅ **Multi-platform packaging** (DEB, RPM, MSI, Homebrew, pip, npm)
- ✅ **GitHub Actions** CI/CD pipelines
- ✅ **Docker** containerization
- ✅ **Infrastructure as Code** principles
- ✅ **Automated testing** and quality gates

## 🚀 Features

### Three Powerful Commands

1. **Config Validation** - Validate DevOps configuration files
   - JSON (with schema detection)
   - YAML (with Ansible/Docker Compose/K8s detection)
   - TOML (Cargo, Terraform)
   - ENV files

2. **Artifact Analysis** - Inspect build artifacts
   - DEB packages (with dependency extraction)
   - RPM packages
   - Docker files (multi-stage detection)
   - Archives (tar, zip, gzip)

3. **Health Checking** - Validate DevOps environment
   - System information (OS, CPU, memory, disk)
   - Tool detection (git, docker, kubectl, ansible, terraform, cmake)
   - Environment variable validation

## 📦 Installation

### Quick Start with Docker (No Installation Required!)

```bash
# Run directly from Docker
docker run --rm -v $(pwd):/workspace ghcr.io/neonix888/devops-validator:latest validate config.json

# Or build locally
docker build -t devops-validator .
docker run --rm -v $(pwd):/workspace devops-validator health
```

### Linux (Debian/Ubuntu)

```bash
# Download and install DEB package
wget https://github.com/neonix888/devops-validator/releases/latest/download/devops-validator-1.0.0-Linux.deb
sudo dpkg -i devops-validator-1.0.0-Linux.deb
```

### Linux (RHEL/CentOS/Fedora)

```bash
# Download and install RPM package
wget https://github.com/neonix888/devops-validator/releases/latest/download/devops-validator-1.0.0-Linux.rpm
sudo rpm -i devops-validator-1.0.0-Linux.rpm
```

### macOS (Homebrew)

```bash
# Add tap and install
brew tap neonix888/devops-validator
brew install devops-validator
```

### Python (pip)

```bash
# Install via pip (downloads platform-specific binary)
pip install devops-validator
```

### Node.js (npm)

```bash
# Install via npm (downloads platform-specific binary)
npm install -g devops-validator
```

### Build from Source

```bash
# Clone repository
git clone https://github.com/neonix888/devops-validator.git
cd devops-validator

# Build with CMake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Install
sudo cmake --install build

# Or create packages
cd build
cpack -G DEB  # Create .deb
cpack -G RPM  # Create .rpm
cpack -G TGZ  # Create .tar.gz
```

## 💻 Usage

### Config Validation

```bash
# Validate single config file
devops-validator validate docker-compose.yml
devops-validator validate ansible-playbook.yml
devops-validator validate .env

# Validate entire directory
devops-validator validate /path/to/configs/

# Example output:
# ✓ Valid YAML file
# ℹ Detected Docker Compose file
# ⚠ WARNING: Docker Compose 'version' field missing
```

### Artifact Analysis

```bash
# Analyze package
devops-validator analyze build.deb
devops-validator analyze app.rpm

# Analyze Dockerfile
devops-validator analyze Dockerfile

# Analyze directory of artifacts
devops-validator analyze /path/to/artifacts/

# Example output:
# Type: DEB Package
# Name: devops-validator-1.0.0-Linux.deb
# Size: 2.45 MB
# Metadata:
#   Package: devops-validator
#   Version: 1.0.0
#   Architecture: amd64
# ✓ Artifact analysis complete
```

### Health Check

```bash
# Check system health
devops-validator health

# Example output:
# System Information:
#   OS: Linux 6.8.0-86-generic
#   CPU: 8 cores
#   Memory: 16 GB
# DevOps Tools:
#   ✓ git: git version 2.43.0
#   ✓ docker: Docker version 24.0.7
#   ✓ kubectl: Client Version: v1.28.0
#   ✓ ansible: ansible [core 2.15.0]
#   ✓ terraform: Terraform v1.6.0
#   ✓ cmake: cmake version 3.28.1
# ✓ System is healthy - all checks passed!
```

## 🏗️ Architecture & DevOps Practices

### Project Structure

```
devops-validator/
├── CMakeLists.txt              # Modern CMake configuration
├── src/                        # C++ source files
│   ├── main.cpp
│   ├── config_validator.cpp
│   ├── artifact_analyzer.cpp
│   ├── health_checker.cpp
│   └── utils.cpp
├── include/                    # Header files
├── tests/                      # Test suite
├── packaging/                  # Multi-platform packages
│   ├── deb/                   # Debian packaging
│   ├── rpm/                   # RPM packaging
│   ├── homebrew/              # Homebrew formula
│   ├── python/                # Python pip wrapper
│   └── npm/                   # npm wrapper
├── .github/workflows/          # CI/CD pipelines
│   └── ci.yml                 # Multi-platform build
├── Dockerfile                  # Container configuration
└── docs/                       # Documentation
```

### CI/CD Pipeline

The GitHub Actions workflow demonstrates:

1. **Multi-Platform Builds**
   - Ubuntu (with DEB, RPM, TGZ packaging)
   - macOS (with TGZ packaging)
   - Windows (with ZIP packaging)

2. **Automated Testing**
   - Unit tests on all platforms
   - Integration tests
   - Smoke tests

3. **Code Quality**
   - cppcheck static analysis
   - clang-format validation
   - Security scanning with Trivy

4. **Docker**
   - Multi-stage build
   - Layer caching
   - Automated testing

5. **Release Automation**
   - Automatic asset upload on release
   - Version tagging
   - Changelog generation

### CMake Best Practices

- ✅ Modern CMake (3.20+)
- ✅ Target-based configuration
- ✅ FetchContent for dependencies
- ✅ CPack for packaging
- ✅ CTest for testing
- ✅ Cross-platform support
- ✅ Install rules
- ✅ Proper library linking

## 🔧 Development

### Prerequisites

- CMake 3.20+
- C++17 compiler (GCC 9+, Clang 10+, MSVC 2019+)
- Git

### Building

```bash
# Debug build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build

# Release build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run tests
cd build && ctest --output-on-failure
```

### Creating Packages

```bash
cd build

# Linux packages
cpack -G DEB
cpack -G RPM
cpack -G TGZ

# Show package info
dpkg-deb -I devops-validator-1.0.0-Linux.deb
```

### Docker Development

```bash
# Build image
docker build -t devops-validator .

# Run tests
docker run --rm devops-validator version
docker run --rm -v $(pwd)/tests:/workspace devops-validator validate /workspace
```

## 🎓 Key Learning Demonstrations

This project showcases:

### 1. CMake Expertise
- Modern CMake practices (target-based, FetchContent)
- Cross-platform build configuration
- CPack integration for multiple package formats
- CTest for automated testing
- Install rules and export targets

### 2. Multi-Platform Packaging
- **DEB**: Debian package with postinst scripts
- **RPM**: Red Hat package with spec-like configuration
- **MSI**: Windows installer (via WiX)
- **Homebrew**: macOS formula
- **pip**: Python wrapper with binary distribution
- **npm**: Node.js wrapper with platform detection

### 3. GitHub Actions CI/CD
- Matrix builds for multiple platforms
- Artifact caching
- Automated testing gates
- Security scanning
- Release automation
- Docker integration

### 4. Docker Best Practices
- Multi-stage builds for minimal image size
- Non-root user execution
- Proper entrypoint configuration
- BuildKit cache optimization

### 5. DevOps Principles
- Infrastructure as Code (Dockerfile, GitHub Actions YAML)
- Automated testing at every commit
- Continuous Integration and Delivery
- Security scanning (Trivy)
- Code quality gates (cppcheck)

## 📊 Technical Stack

- **Language**: C++17
- **Build System**: CMake 3.20+
- **Dependencies**: nlohmann/json, yaml-cpp (via FetchContent)
- **CI/CD**: GitHub Actions
- **Containers**: Docker with multi-stage builds
- **Package Managers**: apt, yum, brew, pip, npm
- **Testing**: CTest

## 🤝 Contributing

This is a demonstration project, but contributions are welcome!

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run tests: `cd build && ctest`
5. Submit a pull request

## 📝 License

MIT License - see [LICENSE](LICENSE) file for details

## 🎯 Why This Project?

This project was built to demonstrate **production-ready DevOps engineering skills**:

✅ **Real-world applicability** - Solves actual DevOps problems (validation, analysis, health checking)

✅ **Professional practices** - Modern CMake, CI/CD, automated testing, security scanning

✅ **Multi-platform support** - Works everywhere (Linux, macOS, Windows, Docker)

✅ **Comprehensive packaging** - Shows expertise in ALL package formats mentioned in job requirements

✅ **Documentation** - Professional README, inline comments, usage examples

✅ **Maintainability** - Clean code structure, modular design, testable components

## 📬 Contact

**GitHub**: [@neonix888](https://github.com/neonix888)

**Repository**: [https://github.com/neonix888/devops-validator](https://github.com/neonix888/devops-validator)

---

*Built with ❤️ to demonstrate DevOps engineering excellence*
