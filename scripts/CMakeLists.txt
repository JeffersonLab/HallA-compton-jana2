#
# CMake configuration for helper scripts (e.g. Valgrind checks)
#

set(VALGRIND_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/valgrind_check.csh")

add_custom_target(valgrindCheck
    COMMAND ${CMAKE_COMMAND} -E echo "Running Valgrind memory check..."
    # Run the script from the project root so relative paths (e.g. build/) match
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
    COMMAND ${VALGRIND_SCRIPT}
    VERBATIM
)


