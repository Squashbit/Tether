cmake_minimum_required (VERSION 3.8)
project ("TetherProject")

# All subdirectories will be compiled with this C++ version
set(CMAKE_CXX_STANDARD 20)

# The relative path to tether from each module directory
# Tether and Tether Modules are both children of the root repo directory,
# so this value shouldn't change.
set(TETHER_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(TETHER_DIR ${TETHER_PROJECT_DIR}/Tether)
set(TETHER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

option(TETHER_BUILD_TESTS "Build tether with tests" OFF)
option(TETHER_BUILD_AS_SHARED "Build tether as a shared library" ON)
option(TETHER_BUILD_RENDERING "Build Tether.Rendering" OFF)

message(STATUS "TETHER_BUILD_TESTS      = ${TETHER_BUILD_TESTS}")
message(STATUS "TETHER_BUILD_AS_SHARED  = ${TETHER_BUILD_AS_SHARED}")

set (TETHER_BIN2CPP_DIR "${TETHER_PROJECT_DIR}/Vendor/Bin2cpp" CACHE STRING
	"bin2cpp directory")
set (TETHER_VMA_DIR "${TETHER_PROJECT_DIR}/Tether.Rendering/Vendor/VMA" CACHE STRING
	"VMA directory")
set (TETHER_FREETYPE_DIR "${TETHER_PROJECT_DIR}/Tether.Rendering/Vendor/freetype" CACHE STRING
	"freetype directory")
set (TETHER_STB_DIR "${TETHER_PROJECT_DIR}/Tether.Rendering/Vendor/stb_image" CACHE STRING
	"stb directory")
set (TETHER_SHADERC_DIR "${TETHER_PROJECT_DIR}/Tether.Rendering/Vendor/shaderc" CACHE STRING
	"shaderc directory")

if (IS_DIRECTORY ${TETHER_BIN2CPP_DIR})
	add_subdirectory(${TETHER_BIN2CPP_DIR} bin2cpp)
endif()

if (TETHER_BUILD_AS_SHARED)
	set(TETHER_LIBRARY_MODE SHARED)
	add_definitions(-D_TETHER_BUILD_DLL)
else()
	set(TETHER_LIBRARY_MODE STATIC)
	add_definitions(-D_TETHER_STATIC)
	add_definitions(-D_TETHER_BUILD_STATIC)
endif()

add_subdirectory("Tether")

if (TETHER_BUILD_RENDERING)
	add_subdirectory("Tether.Rendering")
endif()