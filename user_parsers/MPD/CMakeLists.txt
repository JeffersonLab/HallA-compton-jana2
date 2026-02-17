#
# CMake configuration for MPD user parsers
#

# Library containing MPD bank parser implementation
add_library(mpd_parser STATIC
    BankParser_MPD.cc
)

target_include_directories(mpd_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}       # MPD parser headers
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects  # MPD data object headers
)

# Link against core parser to get classes from which parser classes inherit
target_link_libraries(mpd_parser
    PUBLIC
        parser
)


