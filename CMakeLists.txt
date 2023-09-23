cmake_minimum_required(VERSION 3.5)

project(CppAppTemplate)

set(CMAKE_CXX_STANDARD 14)
 
### Configuration stage ###
# Gather includes
include_directories("inc")

# Configure and enable testing
enable_testing()
add_subdirectory(googletest)

# Gather source
file(GLOB SOURCES "src/example/*.cpp" "src/*.cpp")

### Build stage ###
# Build executable
add_executable(ExampleApp ${SOURCES})

### Install stage ###
# Install executable
install(TARGETS ExampleApp DESTINATION /usr/bin)
