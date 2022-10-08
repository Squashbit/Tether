cmake_minimum_required (VERSION 3.8)
project ("RendererTesting")

set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SOURCES 
	"Source/*.cpp" "Source/*.c"
	"Headers/*.hpp" "Headers/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})
add_dependencies(${PROJECT_NAME} tether tetherRendering)

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} 
PUBLIC 
	"Headers"
	"../../Tether/Headers"
	"../../Tether.Rendering/Headers"
PRIVATE
	${Vulkan_INCLUDE_DIR}
)

if (UNIX)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS}
		${Vulkan_LIBRARIES}
		pthread tether tetherRendering X11
	)
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS}
		${Vulkan_LIBRARIES}
		tether tetherRendering
	)
endif()
	
