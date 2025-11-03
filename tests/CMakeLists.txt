# Test configuration
# For now, we'll add simple smoke tests

# Create a test that just runs the tool
add_test(NAME help_test
         COMMAND devops-validator help)

add_test(NAME version_test
         COMMAND devops-validator version)

# Test with sample config files
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/test.json "{\"version\": \"1.0\"}")
add_test(NAME json_validation_test
         COMMAND devops-validator validate ${CMAKE_CURRENT_BINARY_DIR}/test.json)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/test.yaml "version: 1.0")
add_test(NAME yaml_validation_test
         COMMAND devops-validator validate ${CMAKE_CURRENT_BINARY_DIR}/test.yaml)

# Health check test
add_test(NAME health_check_test
         COMMAND devops-validator health)
