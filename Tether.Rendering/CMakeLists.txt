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

add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE}
    ${HEADERS}

    Source/Objects/Object.cpp 
    Source/Objects/Rectangle.cpp 
    Source/GraphicalWindow.cpp 
    Source/UIRenderer.cpp
    Source/RenderingModule.cpp
)

add_dependencies(${PROJECT_NAME} tether compileTetherRenderingAssets)

set(TETHER_RENDERING_DEPENDENT_SOURCES)
if (TETHER_RENDERING_VULKAN_API)
    set(TETHER_RENDERING_DEPENDENT_SOURCES ${TETHER_RENDERING_DEPENDENT_SOURCES} 
        Source/Vulkan/Common/TypeNames.cpp
        Source/Vulkan/Objects/RectangleRenderer.cpp
        Source/Vulkan/Objects/ObjectRenderer.cpp
        Source/Vulkan/Device.cpp
        Source/Vulkan/DeviceLoader.cpp
        Source/Vulkan/Instance.cpp
        Source/Vulkan/InstanceLoader.cpp
        Source/Vulkan/VulkanUIRenderer.cpp
        Source/Vulkan/Surface.cpp
        Source/Vulkan/Pipeline.cpp
        Source/Vulkan/Swapchain.cpp
        Source/Vulkan/VertexBuffer.cpp
        Source/Vulkan/BufferStager.cpp
        Source/Vulkan/VmaImplementation.cpp
        Source/Vulkan/Vulkan_RenderingModule.cpp
        Source/Vulkan/Allocator.cpp
     "Source/Vulkan/RenderPass.cpp")
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
