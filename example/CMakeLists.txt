set(FLUXINS_EXAMPLES
    usage
    customize
    repl
)

foreach(EXAMPLE ${FLUXINS_EXAMPLES})
    set(EXAMPLE_TARGET fluxins_${EXAMPLE}_example)
    add_executable(${EXAMPLE_TARGET} ${EXAMPLE}.cpp)
    target_link_libraries(${EXAMPLE_TARGET} PRIVATE fluxins)
    target_include_directories(${EXAMPLE_TARGET} PRIVATE ${FLUXINS_SOURCE_DIR}/example)
endforeach()
