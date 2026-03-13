#
# CMake configuration for services
#

add_library(services STATIC
    JEventService_FilterDB.cc
)

target_include_directories(services
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}    # Expose services/ headers
    PRIVATE
        ${JANA_INCLUDE_DIRS}           # JANA2 headers
)

target_link_libraries(services
    PRIVATE
        ${JANA_LIBRARY}                # JANA2 framework library
    PUBLIC
        jce_config                     # Provides jce_config_paths.h to services and their consumers
)
