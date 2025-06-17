include("${FLUXINS_SOURCE_DIR}/cmake/subdep.cmake")

subdep_add(doctest)

set(FLUXINS_TESTS
    basic
    config
    operator
    builtins
    context
    error
)

foreach(TEST ${FLUXINS_TESTS})
    set(TEST_TARGET fluxins_${TEST}_test)
    add_executable(${TEST_TARGET} ${TEST}.cpp)
    target_link_libraries(${TEST_TARGET} PRIVATE fluxins doctest::doctest)
    target_include_directories(${TEST_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME ${TEST} COMMAND ${TEST_TARGET})
endforeach()
