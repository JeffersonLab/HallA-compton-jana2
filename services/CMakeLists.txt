#
# CMake configuration for services
#

add_library(services INTERFACE)

target_include_directories(services
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}   # Expose services/ as an include directory
)

