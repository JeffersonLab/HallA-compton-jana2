#
# CMake configuration for services
#

add_library(services STATIC 
    JEventService_FilterDB.cc
)

target_include_directories(services
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}   # Expose services/ as an include directory
    PRIVATE
        ${JANA_INCLUDE_DIRS}           # JANA2 headers
)

target_link_libraries(services
    PRIVATE
        ${JANA_LIBRARY}                # JANA2 framework library
)

