add_library(fluxins
    fluxins.cpp
    builtins.cpp
    parser.cpp
    evaluator.cpp
    debug.cpp
)
target_include_directories(fluxins PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(fluxins PUBLIC cxx_std_23)

install(TARGETS fluxins
    EXPORT fluxins_EXPORT
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include" DESTINATION "include")
install(EXPORT fluxins_EXPORT
    FILE "fluxins-config.cmake"
    NAMESPACE fluxins::
    DESTINATION "lib/cmake/fluxins"
)
export(TARGETS fluxins
    FILE "${CMAKE_BINARY_DIR}/fluxins_export.cmake"
    NAMESPACE fluxins::
)

configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/fluxins.pc.in"
    "${CMAKE_BINARY_DIR}/cmake/fluxins.pc"
    @ONLY
)
install(FILES "${CMAKE_BINARY_DIR}/cmake/fluxins.pc" DESTINATION "lib/pkgconfig")

add_executable(fluxins_executable
    main.cpp
)
target_link_libraries(fluxins_executable PRIVATE fluxins)

install(TARGETS fluxins_executable
    RUNTIME DESTINATION bin
)
