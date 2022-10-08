cmake_minimum_required (VERSION 3.8)
project ("SimpleWindow")

set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SOURCES 
	"Source/*.cpp" "Source/*.c"
	"Headers/*.hpp" "Headers/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
PUBLIC 
	"Headers" 
	"../../Headers"
	"../../Tether.Rendering/Headers"
)

if (UNIX)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS} 
		pthread tether tetherRendering X11
	)
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS} 
		tether tetherRendering
	)
endif()
	
