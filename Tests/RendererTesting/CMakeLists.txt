cmake_minimum_required (VERSION 3.8)
project ("RendererTesting")

file(GLOB_RECURSE SOURCES 
	"Source/*.cpp" "Source/*.c"
	"Headers/*.hpp" "Headers/*.h"
)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} ${SOURCES})

set(VK_LIBS)
set(VK_INCLUDES)
if (TETHER_VULKAN_API)
    add_dependencies(${PROJECT_NAME} vkshaders VulkanMemoryAllocator)

    set(VK_LIBS ${Vulkan_LIBRARIES})
    set(VK_INCLUDES ${Vulkan_INCLUDE_DIR})
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC 
	"Headers"
	"../../Headers"
PRIVATE
	${VK_INCLUDES}
)

target_link_directories(${PROJECT_NAME}
	PRIVATE "../../Build"
)

if (UNIX)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS} pthread tether X11
		${VK_LIBS}
	)
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS} tether
		${VK_LIBS}
	)
endif()
	
