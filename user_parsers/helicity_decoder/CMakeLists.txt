#
# CMake configuration for helicity decoder user parsers
#

# Library containing helicity decoder bank parser implementation
add_library(helicity_decoder_parsers STATIC
    BankParser_HelicityDecoder.cc
)

target_include_directories(helicity_decoder_parsers
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}       # helicity decoder parser headers
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects  # helicity decoder data object headers
)

# Link against core parser to get classes from which parser classes inherit
target_link_libraries(helicity_decoder_parsers
    PUBLIC
        parser
)


