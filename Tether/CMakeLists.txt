cmake_minimum_required (VERSION 3.8)
project ("tether")

file(GLOB_RECURSE SOURCES 
	"Source/*.cpp" "Source/*.c" 
	"Headers/*.hpp" "Headers/*.h" 
)

add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
PUBLIC 
	"Headers"
)

# Set the sources of platform specific files
if (WIN32)
	file(GLOB_RECURSE PLATFORM 
		"Platform/Win32/*.cpp" "Platform/Win32/*.c" 
	)

	target_sources(${PROJECT_NAME} PRIVATE ${PLATFORM})

	target_link_libraries(${PROJECT_NAME})
elseif (UNIX)
	file(GLOB_RECURSE PLATFORM 
		"Platform/Linux/*.cpp" "Platform/Linux/*.c" 
	)

	target_sources(${PROJECT_NAME} PRIVATE ${PLATFORM})

	target_link_libraries(${PROJECT_NAME}
		X11 Xrandr
	)
endif()
