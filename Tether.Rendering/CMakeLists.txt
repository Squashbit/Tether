cmake_minimum_required (VERSION 3.8)
project ("tetherRendering")

option(TETHER_RENDERING_VULKAN_API "Build tether with Vulkan capabilities" OFF)
option(TETHER_RENDERING_OPENGL_API "Build tether with OpenGL capabilities" OFF)
message(STATUS "TETHER_RENDERING_VULKAN_API = ${TETHER_RENDERING_VULKAN_API}")
message(STATUS "TETHER_RENDERING_OPENGL_API = ${TETHER_RENDERING_OPENGL_API}")

add_subdirectory(Assets)
add_subdirectory(Vendor/freetype)

add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE} "Source/Resources/BufferedImage.cpp" "Source/Objects/Object.cpp" "Source/Objects/Rectangle.cpp" "Source/WindowUI.cpp" "Source/Renderer.cpp" "Source/Objects/Image.cpp" "Source/Vulkan/Objects/Image.cpp" "Source/Resources/Resource.cpp" "Source/ImageLoader.cpp" "Source/StbImageImpl.cpp" "Source/FreetypeLibrary.cpp" "Source/Resources/Font.cpp" "Source/Objects/Text.cpp" "Source/Elements/Element.cpp" "Source/Elements/Division.cpp" "Source/Compositor.cpp" "Source/Elements/Button.cpp" "Source/Elements/ProgressBar.cpp")

add_dependencies(${PROJECT_NAME} tether compileTetherRenderingAssets freetype VulkanMemoryAllocator)

if (TETHER_RENDERING_VULKAN_API)
    find_package(Vulkan REQUIRED)
    target_sources(${PROJECT_NAME} PRIVATE "Source/Vulkan/Common/TypeNames.cpp" "Source/Vulkan/Objects/Rectangle.cpp" "Source/Vulkan/Device.cpp" "Source/Vulkan/DeviceLoader.cpp" "Source/Vulkan/Instance.cpp" "Source/Vulkan/InstanceLoader.cpp" "Source/Vulkan/VulkanRenderer.cpp" "Source/Vulkan/Surface.cpp" "Source/Vulkan/Pipeline.cpp" "Source/Vulkan/Swapchain.cpp" "Source/Vulkan/VertexBuffer.cpp" "Source/Vulkan/BufferStager.cpp" "Source/Vulkan/VmaImplementation.cpp"    "Source/Vulkan/ShaderModule.cpp"  "Source/Vulkan/DescriptorPool.cpp" "Source/Vulkan/UniformBuffer.cpp" "Source/Vulkan/Resources/BufferedImage.cpp"  "Source/Vulkan/SingleUseCommandBuffer.cpp"  "Source/Vulkan/CommandBufferDescriptor.cpp" "Source/Vulkan/DescriptorSet.cpp" "Source/Vulkan/DescriptorSetWritable.cpp" "Source/Vulkan/Objects/Text.cpp" "Source/Vulkan/Resources/Font.cpp" "Source/Vulkan/DebugCallback.cpp" "Source/Vulkan/GlobalVulkan.cpp" "Source/Vulkan/ImageStager.cpp" "Source/Vulkan/VulkanCompositor.cpp"  "Source/Vulkan/AllocatorManager.cpp" "Source/Vulkan/VulkanWindow.cpp")
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC 
    "Headers"
    "Vendor/VMA/include"
    "Vendor/freetype/include"
    "Vendor/stb_image"
PRIVATE
    ${TETHER_BINARY_DIR}
    ${TETHER_DIR}/Headers
    ${Vulkan_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    tether freetype
)
