cmake_minimum_required (VERSION 3.8)
project ("TetherProject")

option(TETHER_BUILD_TESTS "Build tether with tests" OFF)
message(STATUS "TETHER_BUILD_TESTS = ${TETHER_BUILD_TESTS}")

if (TETHER_BUILD_TESTS)
    add_subdirectory("Tests/SimpleWindow")
    add_subdirectory("Tests/RendererTesting")
    add_subdirectory("Tests/MultipleWindows")
endif()

add_subdirectory("Tether")
add_subdirectory("Tether.Rendering")