cmake_minimum_required (VERSION 3.8)
project ("TetherProject")

# All subdirectories will be compiled with this C++ version
set(CMAKE_CXX_STANDARD 17)
# The relative path to tether from each module directory
# Tether and Tether Modules are both children of the root repo directory,
# so this value shouldn't change.
set(TETHER_RELATIVE_PATH "../Tether")
set(TETHER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

option(TETHER_BUILD_TESTS "Build tether with tests" OFF)
option(TETHER_BUILD_AS_SHARED "Build tether as a shared library" ON)
option(
	TETHER_BIN2CPP_EXTERNAL 
	"Should be ON if Bin2cpp is supplied somewhere other than Vendor/Bin2cpp."
	OFF
)
message(STATUS "TETHER_BUILD_TESTS      = ${TETHER_BUILD_TESTS}")
message(STATUS "TETHER_BUILD_AS_SHARED  = ${TETHER_BUILD_AS_SHARED}")
message(STATUS "TETHER_BIN2CPP_EXTERNAL = ${TETHER_BIN2CPP_EXTERNAL}")

if (NOT TETHER_BIN2CPP_EXTERNAL)
	add_subdirectory("Vendor/Bin2cpp")
endif()

if (TETHER_BUILD_AS_SHARED)
    set(TETHER_LIBRARY_MODE SHARED)
    add_definitions(-D_TETHER_BUILD_DLL)
else()
   set(TETHER_LIBRARY_MODE STATIC)
endif()

if (TETHER_BUILD_TESTS)
    add_subdirectory("Tests")
endif()

add_subdirectory("Tether")
add_subdirectory("Tether.Rendering")