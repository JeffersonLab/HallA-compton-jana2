#
# CMake configuration for FADC user parsers
#

# Library containing FADC250 bank parser implementation
add_library(fadc_parsers STATIC
    BankParser_FADC.cc
)

target_include_directories(fadc_parsers
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}       # FADC250 parser headers
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects  # FADC250 data object headers
)

# Link against core parser to get classes from which parser classes inherit
target_link_libraries(fadc_parsers
    PUBLIC
        parser
)


