cmake_minimum_required (VERSION 3.8)
project ("tetherRendering")

option(TETHER_RENDERING_VULKAN_API "Build tether with Vulkan capabilities" OFF)
option(TETHER_RENDERING_OPENGL_API "Build tether with OpenGL capabilities" OFF)
message(STATUS "TETHER_RENDERING_VULKAN_API = ${TETHER_RENDERING_VULKAN_API}")
message(STATUS "TETHER_RENDERING_OPENGL_API = ${TETHER_RENDERING_OPENGL_API}")

add_subdirectory(Assets)

if (TETHER_BUILD_TESTS)
	add_subdirectory("Tests")
endif()

if (IS_DIRECTORY ${TETHER_FREETYPE_DIR})
	add_subdirectory(${TETHER_FREETYPE_DIR} freetype)
	set_property(TARGET freetype PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE} "Source/WindowUIManager.cpp" "Source/Resources/BufferedImage.cpp" "Source/Resources/Resource.cpp" "Source/ImageLoader.cpp" "Source/StbImageImpl.cpp" "Source/FreetypeLibrary.cpp" "Source/Resources/Font.cpp" "Source/Elements/Element.cpp" "Source/Elements/Division.cpp"  "Source/Elements/Button.cpp" "Source/Elements/ProgressBar.cpp" "Source/Elements/Common/BorderedElement.cpp" "Source/Elements/Text.cpp" "Source/Vulkan/VulkanRenderer.cpp" "Source/Elements/ImageView.cpp")

if (TETHER_RENDERING_VULKAN_API)
	find_package(Vulkan REQUIRED)
	target_sources(${PROJECT_NAME} PRIVATE "Source/Vulkan/GraphicsContext.cpp" "Source/Vulkan/Device.cpp" "Source/Vulkan/DeviceLoader.cpp" "Source/Vulkan/Instance.cpp" "Source/Vulkan/InstanceLoader.cpp" "Source/Vulkan/Surface.cpp" "Source/Vulkan/Pipeline.cpp" "Source/Vulkan/Swapchain.cpp" "Source/Vulkan/VertexBuffer.cpp" "Source/Vulkan/BufferStager.cpp" "Source/Vulkan/VmaImplementation.cpp"    "Source/Vulkan/ShaderModule.cpp"  "Source/Vulkan/DescriptorPool.cpp" "Source/Vulkan/UniformBuffer.cpp" "Source/Vulkan/Resources/BufferedImage.cpp"  "Source/Vulkan/SingleUseCommandBuffer.cpp"   "Source/Vulkan/DescriptorSet.cpp" "Source/Vulkan/DescriptorSetWritable.cpp" "Source/Vulkan/Resources/Font.cpp"   "Source/Vulkan/ImageStager.cpp" "Source/Vulkan/AllocatorManager.cpp" "Source/Vulkan/ContextCreator.cpp" "Source/Vulkan/WindowRenderTargetVk.cpp"  "Source/Vulkan/CommandBufferState.cpp")

	if (IS_DIRECTORY ${TETHER_VMA_DIR})
		add_subdirectory(${TETHER_VMA_DIR} vma)
	endif()

	add_dependencies(${PROJECT_NAME} VulkanMemoryAllocator)
	execute_process(COMMAND python3 ${TETHER_SHADERC_DIR}/utils/git-sync-deps)
	set(SHADERC_SKIP_TESTS ON)

	if (IS_DIRECTORY ${TETHER_SHADERC_DIR})
		add_subdirectory(Vendor/shaderc)
	endif()
endif()

add_dependencies(${PROJECT_NAME} tether compileTetherRenderingAssets freetype)

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
