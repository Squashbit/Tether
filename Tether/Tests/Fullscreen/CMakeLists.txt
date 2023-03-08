cmake_minimum_required (VERSION 3.8)
project ("Fullscreen")

add_executable(${PROJECT_NAME} "Source/Main.cpp")

target_include_directories(${PROJECT_NAME} 
PUBLIC 
	"Headers" 
	"../../Headers"
)

if (UNIX)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS} 
		pthread tether X11
	)
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS} 
		tether
	)
endif()
	
