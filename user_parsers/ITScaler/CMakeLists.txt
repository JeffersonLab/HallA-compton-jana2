#
# CMake configuration for IT scaler user parsers
#

# Library containing IT scaler bank parser implementation
add_library(itscaler_parsers STATIC
    BankParser_ITScaler.cc
)

target_include_directories(itscaler_parsers
    PUBLIC
        ${PROJECT_SOURCE_DIR}/user_parsers/ITScaler         # IT scaler parser headers
        ${PROJECT_SOURCE_DIR}/user_parsers/ITScaler/data_objects  # IT scaler data object headers
)

# Link against core parser library
target_link_libraries(itscaler_parsers
    PUBLIC
        parser
)


