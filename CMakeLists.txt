cmake_minimum_required (VERSION 3.8)
project ("TetherProject")

option(TETHER_BUILD_TESTS "Build tether with tests" OFF)
option(TETHER_BUILD_AS_SHARED "Build tether as a shared library" ON)

message(STATUS "TETHER_BUILD_TESTS     = ${TETHER_BUILD_TESTS}")
message(STATUS "TETHER_BUILD_AS_SHARED = ${TETHER_BUILD_AS_SHARED}")

if (TETHER_BUILD_AS_SHARED)
    set(TETHER_LIBRARY_MODE SHARED)
    add_definitions(-D_TETHER_BUILD_DLL)
else()
   set(TETHER_LIBRARY_MODE STATIC)
endif()

if (TETHER_BUILD_TESTS)
    add_subdirectory("Tests/SimpleWindow")
    add_subdirectory("Tests/RendererTesting")
    add_subdirectory("Tests/MultipleWindows")
endif()

add_subdirectory("Tether")
add_subdirectory("Tether.Rendering")