cmake_minimum_required (VERSION 3.8)
project ("tetherRendering")

option(TETHER_RENDERING_VULKAN_API "Build tether with Vulkan capabilities" OFF)
option(TETHER_RENDERING_OPENGL_API "Build tether with OpenGL capabilities" OFF)
message(STATUS "TETHER_RENDERING_VULKAN_API = ${TETHER_RENDERING_VULKAN_API}")
message(STATUS "TETHER_RENDERING_OPENGL_API = ${TETHER_RENDERING_OPENGL_API}")

# Add the assets CMakeLists.txt file
add_subdirectory(Assets)

add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE} "Source/Objects/Object.cpp" "Source/Objects/Rectangle.cpp" "Source/GraphicalWindow.cpp" "Source/Renderer.cpp" "Source/RenderingModule.cpp" "Source/Objects/Image.cpp" "Source/Vulkan/Objects/Image.cpp" "Source/Resources/Resource.cpp" "Source/ImageLoader.cpp" "Source/StbImageImpl.cpp")

add_dependencies(${PROJECT_NAME} tether compileTetherRenderingAssets)

set(TETHER_RENDERING_DEPENDENT_SOURCES)
if (TETHER_RENDERING_VULKAN_API)
    set(TETHER_RENDERING_DEPENDENT_SOURCES ${TETHER_RENDERING_DEPENDENT_SOURCES} "Source/Vulkan/Common/TypeNames.cpp" "Source/Vulkan/Objects/Rectangle.cpp" "Source/Vulkan/Device.cpp" "Source/Vulkan/DeviceLoader.cpp" "Source/Vulkan/Instance.cpp" "Source/Vulkan/InstanceLoader.cpp" "Source/Vulkan/VulkanRenderer.cpp" "Source/Vulkan/Surface.cpp" "Source/Vulkan/Pipeline.cpp" "Source/Vulkan/Swapchain.cpp" "Source/Vulkan/VertexBuffer.cpp" "Source/Vulkan/BufferStager.cpp" "Source/Vulkan/VmaImplementation.cpp" "Source/Vulkan/Vulkan_RenderingModule.cpp" "Source/Vulkan/Allocator.cpp" "Source/Vulkan/RenderPass.cpp" "Source/Vulkan/ShaderModule.cpp"  "Source/Vulkan/DescriptorPool.cpp" "Source/Vulkan/UniformBuffer.cpp" "Source/Vulkan/VulkanBufferedImage.cpp"  "Source/Vulkan/SingleUseCommandBuffer.cpp"  "Source/Vulkan/CommandBufferDescriptor.cpp" "Source/Vulkan/DescriptorSet.cpp" "Source/Vulkan/DescriptorSetWritable.cpp" )
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
    "../Vendor/stb_image"
)

target_link_libraries(${PROJECT_NAME}
    tether
)
