#
# Central configuration for all user parser libraries
#

# Add individual user parser subdirectories
add_subdirectory(FADC)
add_subdirectory(FADCScaler)
add_subdirectory(ITScaler)

# Collect all user parser libraries in a single variable that the top-level
# CMakeLists.txt can link against. When adding a new parser library, extend
# this list here instead of touching the main CMakeLists.txt
set(USER_PARSER_LIBS
    fadc_parsers
    fadc_scaler_parsers
    itscaler_parsers
    PARENT_SCOPE
)


