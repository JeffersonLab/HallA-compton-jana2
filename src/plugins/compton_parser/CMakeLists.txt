add_subdirectory(module_parsers)

add_jana_plugin(compton_parser
    SOURCES
        InitPlugin.cc
        module_parsers/InitModuleParsers.cc
    PUBLIC_HEADER
        ${MODULE_PARSERS_HEADERS}
)

target_link_libraries(compton_parser
    PRIVATE
        ${JANA_LIBRARY}
        jana2_common_extensions::evio_parser_api
        ${MODULE_PARSERS_LIBS}
)

add_library(compton_data_types INTERFACE)
target_link_libraries(compton_data_types INTERFACE jana2_common_extensions::evio_parser_api)
foreach(_dir IN LISTS MODULE_PARSERS_INCLUDE_DIRS)
    target_include_directories(compton_data_types INTERFACE
        $<BUILD_INTERFACE:${_dir}>
    )
endforeach()
target_include_directories(compton_data_types INTERFACE
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/compton_parser>
)

install(TARGETS compton_data_types
    EXPORT comptonTargets
)
