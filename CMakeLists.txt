cmake_minimum_required(VERSION 3.20)
project(devops-validator
    VERSION 1.0.0
    DESCRIPTION "Multi-platform DevOps validation CLI tool"
    LANGUAGES CXX)

# Modern C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build options
option(BUILD_TESTING "Build tests" ON)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

# Platform detection
if(WIN32)
    set(PLATFORM_NAME "Windows")
elseif(APPLE)
    set(PLATFORM_NAME "macOS")
elseif(UNIX)
    set(PLATFORM_NAME "Linux")
endif()

message(STATUS "Building for ${PLATFORM_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Compiler warnings (only for our code, not dependencies)
if(ENABLE_WARNINGS)
    if(MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall -Wextra -pedantic)
    endif()
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Third-party dependencies (header-only JSON library)
include(FetchContent)

# nlohmann/json for JSON parsing
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(json)

# yaml-cpp for YAML parsing
# Use master branch which has updated CMake requirements
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

# Disable yaml-cpp tests and tools to speed up build
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(yaml-cpp)

# Source files
set(SOURCES
    src/main.cpp
    src/config_validator.cpp
    src/artifact_analyzer.cpp
    src/health_checker.cpp
    src/utils.cpp
)

# Headers
set(HEADERS
    include/config_validator.h
    include/artifact_analyzer.h
    include/health_checker.h
    include/utils.h
)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json yaml-cpp::yaml-cpp)

# Installation
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT runtime
)

# Install man page
install(FILES ${CMAKE_SOURCE_DIR}/docs/devops-validator.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
    COMPONENT documentation
    OPTIONAL
)

# CPack configuration for multi-platform packaging
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "devops-validator")
set(CPACK_PACKAGE_VENDOR "neonix888")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Multi-platform DevOps validation CLI tool")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "neonix888@github.com")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/neonix888/devops-validator")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# DEB package configuration
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "neonix888 <neonix888@github.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.34)")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/packaging/deb/postinst")

# RPM package configuration
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.34")
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/packaging/rpm/postinst.sh")

# Windows MSI configuration
if(WIN32)
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_UPGRADE_GUID "12345678-1234-1234-1234-123456789012")
    set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/packaging/msi/icon.ico")
    set(CPACK_WIX_UI_BANNER "${CMAKE_SOURCE_DIR}/packaging/msi/banner.bmp")
    set(CPACK_WIX_UI_DIALOG "${CMAKE_SOURCE_DIR}/packaging/msi/dialog.bmp")
endif()

# macOS configuration
if(APPLE)
    set(CPACK_GENERATOR "productbuild")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-macOS")
endif()

# Default generators for each platform
if(NOT CPACK_GENERATOR)
    if(WIN32)
        set(CPACK_GENERATOR "ZIP;WIX")
    elseif(APPLE)
        set(CPACK_GENERATOR "TGZ;productbuild")
    else()
        set(CPACK_GENERATOR "TGZ;DEB;RPM")
    endif()
endif()

include(CPack)

# Testing
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Print configuration summary
message(STATUS "==============================================")
message(STATUS "  ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "==============================================")
message(STATUS "  Platform: ${PLATFORM_NAME}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Package generators: ${CPACK_GENERATOR}")
message(STATUS "==============================================")
