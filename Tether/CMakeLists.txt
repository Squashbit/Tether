cmake_minimum_required (VERSION 3.8)
project ("tether")

file(GLOB_RECURSE HEADERS 
    "Headers/*.hpp" "Headers/*.h" 
)

set(SDIR Source)
add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE} 
    ${HEADERS}

    ${SDIR}/Common/Color.cpp ${SDIR}/Common/IDisposable.cpp 
    ${SDIR}/Common/StringUtils.cpp ${SDIR}/Controls/Control.cpp
    ${SDIR}/Devices/DeviceManager.cpp ${SDIR}/Devices/Monitor.cpp
    ${SDIR}/Events/EventHandler.cpp ${SDIR}/Events/WindowErrorEvent.cpp
    ${SDIR}/Events/WindowMoveEvent.cpp ${SDIR}/Events/WindowResizeEvent.cpp
    ${SDIR}/Input/InputListener.cpp ${SDIR}/Input/KeyCharInfo.cpp 
    ${SDIR}/Input/KeyInfo.cpp ${SDIR}/Input/MouseMoveInfo.cpp 
    ${SDIR}/Input/RawMouseMoveInfo.cpp ${SDIR}/Application.cpp 
    ${SDIR}/SimpleWindow.cpp ${SDIR}/Module.cpp 
)

# Set the sources of platform specific files
if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SDIR}/Devices/Win32_DeviceManager.cpp
        ${SDIR}/Common/Win32_LibraryLoader.cpp
        ${SDIR}/Win32_Application.cpp
        ${SDIR}/Win32_SimpleWindow.cpp
    )
elseif (UNIX)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SDIR}/Devices/linux_DeviceManager.cpp
        ${SDIR}/Common/Linux_LibraryLoader.cpp
        ${SDIR}/X11_Application.cpp
        ${SDIR}/X11_SimpleWindow.cpp
    )
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC 
    "Headers"
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME})
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME}
        X11 Xrandr
    )
endif()
