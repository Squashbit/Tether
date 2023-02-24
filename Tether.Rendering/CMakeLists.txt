cmake_minimum_required (VERSION 3.8)
project ("tetherRendering")

option(TETHER_RENDERING_VULKAN_API "Build tether with Vulkan capabilities" OFF)
option(TETHER_RENDERING_OPENGL_API "Build tether with OpenGL capabilities" OFF)
message(STATUS "TETHER_RENDERING_VULKAN_API = ${TETHER_RENDERING_VULKAN_API}")
message(STATUS "TETHER_RENDERING_OPENGL_API = ${TETHER_RENDERING_OPENGL_API}")

add_subdirectory(Vendor/ImGui/freetype)

add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE} "Source/ImageLoader.cpp" "Source/StbImageImpl.cpp" "Source/FreetypeLibrary.cpp" "Vendor/ImGui/imgui.cpp" "Vendor/ImGui/imgui_draw.cpp" "Vendor/ImGui/imgui_tables.cpp" "Vendor/ImGui/imgui_widgets.cpp")
add_dependencies(${PROJECT_NAME} tether)

if (TETHER_RENDERING_VULKAN_API)
    find_package(Vulkan REQUIRED)
    target_sources(${PROJECT_NAME} PRIVATE "Source/Vulkan/Device.cpp" "Source/Vulkan/DeviceLoader.cpp" "Source/Vulkan/Instance.cpp" "Source/Vulkan/InstanceLoader.cpp"  "Source/Vulkan/Surface.cpp"  "Source/Vulkan/Swapchain.cpp"   "Source/Vulkan/VmaImplementation.cpp"          "Source/Vulkan/DebugCallback.cpp" "Source/Vulkan/Context.cpp"  "Source/Vulkan/Renderer.cpp"   "Source/Vulkan/VulkanWindow.cpp" "Vendor/ImGui/backends/imgui_impl_vulkan.cpp")
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC 
    "Headers"
    "Vendor/ImGui/VMA/include"
    "Vendor/ImGui/freetype/include"
    "Vendor/ImGui/stb_image"
PRIVATE
    ${TETHER_BINARY_DIR}
    ${TETHER_DIR}/Headers
    ${Vulkan_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    tether freetype
)
