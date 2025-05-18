function(ronminial_add DEP_NAME)
    string(TOUPPER "${DEP_NAME}" DEP_NAME_UPPER)

    # 1. Use custom path
    if(DEFINED "${DEP_NAME_UPPER}_PATH")
        message(STATUS "Adding dependency '${DEP_NAME}' from '${${DEP_NAME_UPPER}_PATH}' (specified by ${DEP_NAME_UPPER}_PATH)")
        add_subdirectory("${${DEP_NAME_UPPER}_PATH}" ${DEP_NAME})
        return()
    endif()

    # 2. Use initialized submodule
    if(EXISTS "${CMAKE_SOURCE_DIR}/deps/${DEP_NAME}/.git")
        message(STATUS "Adding dependency '${DEP_NAME}' from '${CMAKE_SOURCE_DIR}/deps/${DEP_NAME}' (initialized git submodule)")
        add_subdirectory("${CMAKE_SOURCE_DIR}/deps/${DEP_NAME}" ${DEP_NAME})
        return()
    endif()

    # 3. Use system-installed
    find_package(${DEP_NAME} QUIET)
    if(${DEP_NAME}_FOUND)
        message(STATUS "Adding dependency '${DEP_NAME}' (system-installed)")
        return()
    endif()

    # 4. Initialize git submodule
    message(STATUS "Initializing git submodule for '${DEP_NAME}' at '${CMAKE_SOURCE_DIR}/deps/${DEP_NAME}'")
    execute_process(
        COMMAND git submodule update --init --recursive --depth=1 "deps/${DEP_NAME}"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE result
    )
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Failed to initialize submodule for '${DEP_NAME}'")
    endif()
    add_subdirectory("${CMAKE_SOURCE_DIR}/deps/${DEP_NAME}" ${DEP_NAME})
endfunction()