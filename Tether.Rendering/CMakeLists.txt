cmake_minimum_required (VERSION 3.8)
project ("tetherRendering")

set(SDIR Source)
set(CMAKE_CXX_STANDARD 17)
set(TETHER_PATH "../Tether")

option(TETHER_RENDERING_VULKAN_API "Build tether with Vulkan capabilities" OFF)
option(TETHER_RENDERING_OPENGL_API "Build tether with OpenGL capabilities" OFF)

message(STATUS "TETHER_RENDERING_VULKAN_API = ${TETHER_RENDERING_VULKAN_API}")
message(STATUS "TETHER_RENDERING_OPENGL_API = ${TETHER_RENDERING_OPENGL_API}")

add_subdirectory(Assets)

file(GLOB_RECURSE HEADERS 
    "Headers/*.hpp" "Headers/*.h" 
)

add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE}
    ${HEADERS}

    ${SDIR}/GraphicalWindow.cpp 
    ${SDIR}/RenderContext.cpp
    ${SDIR}/RenderingModule.cpp
)

add_dependencies(${PROJECT_NAME} tether compileTetherAssets)

set(DEPENDENT_SOURCES)
if (TETHER_RENDERING_VULKAN_API)
    set(DEPENDENT_SOURCES ${DEPENDENT_SOURCES} 
        ${SDIR}/Vulkan/Device.cpp
        ${SDIR}/Vulkan/DeviceLoader.cpp
        ${SDIR}/Vulkan/Instance.cpp
        ${SDIR}/Vulkan/InstanceLoader.cpp
        ${SDIR}/Vulkan/SimpleNative.cpp
        ${SDIR}/Vulkan/AdvancedNative.cpp
        ${SDIR}/Vulkan/Surface.cpp
        ${SDIR}/Vulkan/Swapchain.cpp
        
        ${SDIR}/Vulkan/Vulkan_RenderingModule.cpp
        ${SDIR}/Vulkan/VkUtils.cpp
    )
endif()

# Set the sources of platform specific files
if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE
        ${DEPENDENT_SOURCES}
    )
elseif (UNIX)
    target_sources(${PROJECT_NAME} PRIVATE
        ${DEPENDENT_SOURCES}
    )
endif()

if (TETHER_RENDERING_VULKAN_API)
    find_package(Vulkan REQUIRED)
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC 
    "Headers"
PRIVATE
    ${TETHER_PATH}/Headers
    ${Vulkan_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    tether
)
