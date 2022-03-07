cmake_minimum_required (VERSION 3.8)
project ("SimpleWindow")

file(GLOB_RECURSE SOURCES 
	"Source/*.cpp" "Source/*.c"
	"Headers/*.hpp" "Headers/*.h"
)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DTETHER_PREVIEW_FEATURES -DTETHER_MONITORS -DTETHER_XRAWINPUT)
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
	PUBLIC "Headers"
	PUBLIC "../../Headers"
)

target_link_directories(${PROJECT_NAME}
	PRIVATE "../../Build"
)

if (UNIX)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS} pthread tether X11 Xrandr Xi
	)
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS} tether
	)
endif()
	
