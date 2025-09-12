cmake_minimum_required(VERSION 3.16)
project(moller LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find JANA library
find_package(JANA REQUIRED)

# Find EVIO library
find_package(evio REQUIRED)

# Specify source files
set(SOURCES
    moller.cc
    JEventSource_EVIO.cc
    JEventProcessor_MOLLER.cc
    JFactory_FADC250.cc
)

# Create executable
add_executable(moller ${SOURCES})

# Include directories
target_include_directories(moller PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link libraries
target_link_libraries(moller
    PRIVATE
        ${JANA_LIBRARY}
        evio::eviocc
)

