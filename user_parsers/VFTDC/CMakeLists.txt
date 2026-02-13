#
# CMake configuration for VFTDC user parsers
#

# Library containing VFTDC bank parser implementation
add_library(vftdc_parser STATIC
    BankParser_VFTDC.cc
)

target_include_directories(vftdc_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}       # VFTDC parser headers
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects  # VFTDC data object headers
)

# Link against core parser to get classes from which parser classes inherit
target_link_libraries(vftdc_parser
    PUBLIC
        parser
)
