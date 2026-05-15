#
# CMake configuration for compton_processor plugin
#
find_package(JANA REQUIRED)

add_jana_plugin(compton_processor
    SOURCES
        InitPlugin.cc
        JEventProcessor_compton.cc
)

target_link_libraries(compton_processor
    PRIVATE
        ${JANA_LIBRARY}
        ROOT::Core
        ROOT::RIO
        ROOT::Tree
        ROOT::Hist
        jana2_common_extensions::evio_parser_data_types  # data-object headers (FADC, TIScaler, MPD, VFTDC, ...)
)
