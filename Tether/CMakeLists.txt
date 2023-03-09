cmake_minimum_required (VERSION 3.8)
project ("tether")

add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE} "Source/Application.cpp" "Source/Common/HashedString.cpp"  "Source/Common/Stopwatch.cpp" "Source/Common/StringTools.cpp" "Source/Controls/Control.cpp" "Source/Devices/Monitor.cpp"  "Source/Events/WindowMoveEvent.cpp" "Source/Events/WindowResizeEvent.cpp" "Source/Input/KeyCharInfo.cpp" "Source/Input/KeyInfo.cpp" "Source/Input/MouseMoveInfo.cpp" "Source/Input/RawMouseMoveInfo.cpp" "Source/Module.cpp" "Source/Platform/CreateWindow.cpp"  "Source/Window.cpp" "Source/Common/Stopwatch.cpp"  "Source/Input/MouseClickInfo.cpp")

target_include_directories(${PROJECT_NAME} 
PUBLIC 
	"Headers"
)

if (WIN32)
	target_sources(${PROJECT_NAME} PRIVATE "Source/Platform/Win32Application.cpp" "Source/Platform/Win32_Library.cpp" "Source/Platform/Win32Window.cpp" )
elseif (UNIX)
	target_sources(${PROJECT_NAME} PRIVATE "Source/Platform/X11Application.cpp" "Source/Platform/Linux_Library.cpp" "Source/Platform/X11Window.cpp")

	target_link_libraries(${PROJECT_NAME}
		X11 Xrandr
	)
endif()
