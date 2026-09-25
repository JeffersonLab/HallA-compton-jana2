#
# Central configuration for all module parser libraries
#

# Add individual module parser subdirectories
add_subdirectory(faV3_compton)

# Collect all module parser libraries in a single variable that the top-level
# CMakeLists.txt can link against. When adding a new parser library, extend
# this list here instead of touching the main CMakeLists.txt
set(MODULE_PARSERS_LIBS
    faV3compton_parser
    PARENT_SCOPE
)

# Collect all module parser include directories. When adding a new parser, add its
# *_INCLUDE_DIR variable here instead of touching the main CMakeLists.txt
set(MODULE_PARSERS_INCLUDE_DIRS
    ${FAV3COMPTON_INCLUDE_DIR}
    PARENT_SCOPE
)

# Collect all module parser public headers. When adding a new parser, add its
# *_PUBLIC_HEADERS variable here instead of touching the main CMakeLists.txt
set(MODULE_PARSERS_HEADERS
    ${FAV3COMPTON_PUBLIC_HEADERS}
    PARENT_SCOPE
)
