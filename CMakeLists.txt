cmake_minimum_required(VERSION 3.5)

project(NathanRockwood)

set(CMAKE_CXX_STANDARD 14)

# Turn unit tests ON or OFF
option(built_unit_tests "Build the unit tests." ON)
 
### Configuration stage ###
# Gather local includes
include_directories("inc")

# Build unit tests if enabled
if(built_unit_tests)
    enable_testing()
    add_subdirectory(test)
endif()

# Configure open-space-toolkit-core library
# add_subdirectory(external/openSpaceToolkitCore)

# Gather source
file(GLOB SOURCES "src/**/*.cpp" "src/*.cpp")

### Build stage ###
# Build executable
add_executable(TrafficLightControllerApp ${SOURCES})

### Install stage ###
# Install executable
install(TARGETS TrafficLightControllerApp DESTINATION /usr/bin)
