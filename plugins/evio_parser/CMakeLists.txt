#
# CMake configuration for evio_parser plugin
#

# Build sub-libraries first (sets PARSER_INCLUDE_DIR, PARSER_PUBLIC_HEADERS,
# USER_PARSER_INCLUDE_DIRS, USER_PARSER_HEADERS, USER_PARSER_LIBS in this scope)
add_subdirectory(services)
add_subdirectory(parser)
add_subdirectory(user_parsers)

# Build the JANA2 plugin (shared library loaded at runtime via -Pplugins=evio_parser)
add_jana_plugin(evio_parser
    SOURCES
        InitPlugin.cc
        JEventSource_EVIO.cc
    PUBLIC_HEADER
        ${PARSER_PUBLIC_HEADERS}
        ${USER_PARSER_HEADERS}
)

target_link_libraries(evio_parser
    PRIVATE
        ${JANA_LIBRARY}
        evio::eviocc
        parser              # EvioEventParser + data objects
        services            # JEventService_FilterDB, BankToModuleMap, ModuleParsersMap
        ${USER_PARSER_LIBS} # fadc_parser, fadc_scaler_parser, tiscaler_parser, ...
)

# ── Exportable INTERFACE target ────────────────────────────────────────────────
# Exposes all data-type headers so downstream plugins can simply do:
#   target_link_libraries(my_plugin PRIVATE evio_parser_data_types)
#
# Generator expressions handle the build/install path split automatically:
#   - BUILD_INTERFACE  → source-tree data_objects/ directories (used during cmake build)
#   - INSTALL_INTERFACE → flat installed include dir  (used after make install)
# No caller ever needs to know or hardcode any path.

add_library(evio_parser_data_types INTERFACE)

foreach(_dir IN LISTS PARSER_INCLUDE_DIR USER_PARSER_INCLUDE_DIRS)
    target_include_directories(evio_parser_data_types INTERFACE
        $<BUILD_INTERFACE:${_dir}>
    )
endforeach()

# add_jana_plugin installs all PUBLIC_HEADERs flat into this directory
target_include_directories(evio_parser_data_types INTERFACE
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/evio_parser>
)

# Register into the project-wide export set; the install(EXPORT ...) call that
# writes the combined targets file lives in the root CMakeLists.txt.
install(TARGETS evio_parser_data_types
    EXPORT jana2_common_extensionsTargets
)
