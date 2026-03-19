#
# CMake configuration for core parser utilities
#

add_library(parser STATIC
    EvioEventParser.cc
)

target_link_libraries(parser
    PUBLIC
        services          # Service utilities
        evio::eviocc      # EVIO C++ library for data I/O
        ${JANA_LIBRARY}   # JANA2 framework library
)

target_include_directories(parser
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
)

set(PARSER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB PARSER_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
)

set(PARSER_PUBLIC_HEADERS ${PARSER_PUBLIC_HEADERS} PARENT_SCOPE)
