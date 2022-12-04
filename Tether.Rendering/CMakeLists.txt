cmake_minimum_required (VERSION 3.8)
project ("tetherRendering")

option(TETHER_RENDERING_VULKAN_API "Build tether with Vulkan capabilities" OFF)
option(TETHER_RENDERING_OPENGL_API "Build tether with OpenGL capabilities" OFF)
message(STATUS "TETHER_RENDERING_VULKAN_API = ${TETHER_RENDERING_VULKAN_API}")
message(STATUS "TETHER_RENDERING_OPENGL_API = ${TETHER_RENDERING_OPENGL_API}")

# Add the assets CMakeLists.txt file
add_subdirectory(Assets)

file(GLOB_RECURSE HEADERS 
    "Headers/*.hpp" "Headers/*.h" 
)

set(SDIR Source)
add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE}
    ${HEADERS}

    ${SDIR}/Objects/Object.cpp 
    ${SDIR}/Objects/Rectangle.cpp 
    ${SDIR}/GraphicalWindow.cpp 
    ${SDIR}/UIRenderer.cpp
    ${SDIR}/RenderingModule.cpp
)

add_dependencies(${PROJECT_NAME} tether compileTetherRenderingAssets)

set(TETHER_RENDERING_DEPENDENT_SOURCES)
if (TETHER_RENDERING_VULKAN_API)
    set(TETHER_RENDERING_DEPENDENT_SOURCES ${TETHER_RENDERING_DEPENDENT_SOURCES} 
        ${SDIR}/Vulkan/ObjectNatives/RectangleNative.cpp
        ${SDIR}/Vulkan/ObjectNatives/VkObjectNative.cpp
        ${SDIR}/Vulkan/Device.cpp
        ${SDIR}/Vulkan/DeviceLoader.cpp
        ${SDIR}/Vulkan/Instance.cpp
        ${SDIR}/Vulkan/InstanceLoader.cpp
        ${SDIR}/Vulkan/SimpleNative.cpp
        ${SDIR}/Vulkan/ContextNative.cpp
        ${SDIR}/Vulkan/Surface.cpp
        ${SDIR}/Vulkan/Pipeline.cpp
        ${SDIR}/Vulkan/Swapchain.cpp
        ${SDIR}/Vulkan/ShaderModule.cpp
        ${SDIR}/Vulkan/VertexBuffer.cpp
        ${SDIR}/Vulkan/BufferStager.cpp
        ${SDIR}/Vulkan/VkUtils.cpp
        ${SDIR}/Vulkan/VmaImplementation.cpp
        ${SDIR}/Vulkan/Vulkan_RenderingModule.cpp
    )
endif()

# Set the sources of platform specific files
if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE
        ${TETHER_RENDERING_DEPENDENT_SOURCES}
    )
elseif (UNIX)
    target_sources(${PROJECT_NAME} PRIVATE
        ${TETHER_RENDERING_DEPENDENT_SOURCES}
    )
endif()

if (TETHER_RENDERING_VULKAN_API)
    find_package(Vulkan REQUIRED)
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC 
    "Headers"
PRIVATE
    ${TETHER_RELATIVE_PATH}/Headers
    ${TETHER_BINARY_DIR}
    ${Vulkan_INCLUDE_DIR}
    "../Vendor/VMA/include"
)

target_link_libraries(${PROJECT_NAME}
    tether
)
