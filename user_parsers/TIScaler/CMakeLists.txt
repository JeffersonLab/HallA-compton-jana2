#
# CMake configuration for TI scaler user parsers
#

# Library containing TI scaler bank parser implementation
add_library(tiscaler_parsers STATIC
    BankParser_TIScaler.cc
)

target_include_directories(tiscaler_parsers
    PUBLIC
        ${PROJECT_SOURCE_DIR}/user_parsers/TIScaler         # TI scaler parser headers
        ${PROJECT_SOURCE_DIR}/user_parsers/TIScaler/data_objects  # TI scaler data object headers
)

# Link against core parser library
target_link_libraries(tiscaler_parsers
    PUBLIC
        parser
)


