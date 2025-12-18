#
# CMake configuration for FADC scaler user parsers
#

# Library containing FADC scaler bank parser implementation
add_library(fadc_scaler_parsers STATIC
    BankParser_FADCScaler.cc
)

target_include_directories(fadc_scaler_parsers
    PUBLIC
        ${PROJECT_SOURCE_DIR}/user_parsers/FADCScaler       # FADC scaler parser headers
        ${PROJECT_SOURCE_DIR}/user_parsers/FADCScaler/data_objects  # FADC scaler data object headers
)

# Link against core parser library
target_link_libraries(fadc_scaler_parsers
    PUBLIC
       parser
)


