#
# CMake configuration for core parser utilities
#

add_library(parser STATIC
    EvioEventParser.cc
)

target_link_libraries(parser
    PUBLIC
        services          # Service utilities (e.g. bank parser map)
        evio::eviocc      # EVIO C++ library for data I/O
        ${JANA_LIBRARY}   # JANA2 framework library
)

target_include_directories(parser
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
    PUBLIC ${PROJECT_SOURCE_DIR}
)
