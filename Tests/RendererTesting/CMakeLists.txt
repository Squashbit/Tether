cmake_minimum_required (VERSION 3.8)
project ("RendererTesting")

file(GLOB_RECURSE SOURCES 
	"Source/*.cpp" "Source/*.c"
	"Headers/*.hpp" "Headers/*.h"
)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} ${SOURCES})

add_dependencies(${PROJECT_NAME} vkshaders VulkanMemoryAllocator)
set(VK_LIBS ${Vulkan_LIBRARIES})
set(VK_INCLUDES ${Vulkan_INCLUDE_DIR})

target_include_directories(${PROJECT_NAME} 
PUBLIC 
	"Headers"
	"../../Tether/Headers"
	"../../Tether.Rendering/Headers"
PRIVATE
	${VK_INCLUDES}
)

if (UNIX)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS} pthread tether tetherRendering X11
		${VK_LIBS}
	)
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS}
		${VK_LIBS}
		tether tetherRendering
	)
endif()
	
