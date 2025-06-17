if(NOT "${FLUXINS_SOURCE_DIR}/documentation" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    find_package(Doxygen QUIET)
    if(Doxygen_FOUND)
        configure_file(
            "${FLUXINS_SOURCE_DIR}/documentation/Doxyfile.in"
            "${FLUXINS_BINARY_DIR}/documentation/Doxyfile"
            @ONLY
        )
        add_custom_target(documentation
            COMMAND ${CMAKE_COMMAND} -E echo "Generating documentation..."
            COMMAND ${DOXYGEN_EXECUTABLE} "${FLUXINS_BINARY_DIR}/Doxyfile"
            WORKING_DIRECTORY "${FLUXINS_SOURCE_DIR}"
            COMMENT "Generate API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()
