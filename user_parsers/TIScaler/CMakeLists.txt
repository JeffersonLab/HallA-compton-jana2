#
# CMake configuration for TI scaler user parsers
#

# Library containing TI scaler module parser implementation
add_library(tiscaler_parser STATIC
    ModuleParser_TIScaler.cc
)

target_include_directories(tiscaler_parser
    PUBLIC
        ${PROJECT_SOURCE_DIR}/user_parsers/TIScaler         # TI scaler parser headers
        ${PROJECT_SOURCE_DIR}/user_parsers/TIScaler/data_objects  # TI scaler data object headers
)

# Link against core parser library
target_link_libraries(tiscaler_parser
    PUBLIC
        parser
)


