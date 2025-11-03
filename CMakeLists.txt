cmake_minimum_required(VERSION 3.16)
project(compton LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find JANA library
find_package(JANA REQUIRED)

# Find EVIO library
# EVIO provides support for reading EVIO format data files
find_package(evio REQUIRED)

# Find ROOT
# ROOT provides TFile, TTree, and histogram functionality
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree Hist)

# Specify source files
set(SOURCES
    compton.cc                    # Main application entry point
    JEventSource_EVIO.cc         # Custom EVIO event source
    JEventProcessor_MOLLER.cc    # Main event processor
    JFactory_FADC250.cc          # Factory for FADC250 detector data
    parsers/EvioEventParser.cc   # EVIO event parsing utilities
    parsers/RawDataParser.cc     # Raw data parsing utilities
)

# Create the main executable
add_executable(compton ${SOURCES})

# Include directories
target_include_directories(compton
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}           # Main source directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects  # Data object definitions
        ${CMAKE_CURRENT_SOURCE_DIR}/parsers       # Parser utilities
)

# Link required libraries to the executable
target_link_libraries(compton
    PRIVATE
        ${JANA_LIBRARY}      # JANA2 framework library
        evio::eviocc         # EVIO C++ library for data I/O
        ROOT::Core           # ROOT core functionality
        ROOT::RIO            # ROOT file I/O (TFile)
        ROOT::Tree           # ROOT TTree functionality
        ROOT::Hist           # ROOT histogram functionality (TH1)
)

