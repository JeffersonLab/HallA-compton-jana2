cmake_minimum_required(VERSION 3.16)
project(compton)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Override with -DINSTALL_NAMESPACE=my_name at cmake configure time if needed
set(INSTALL_NAMESPACE "" CACHE STRING
    "Subdirectory namespace used in all install paths (include/, lib/cmake/, config/, etc.)" FORCE)

# Find dependencies
find_package(JANA REQUIRED)
find_package(jana2_common_extensions REQUIRED)
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree Hist)

# Add subdirectories
#add_subdirectory(config)
add_subdirectory(src/plugins)
