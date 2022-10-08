cmake_minimum_required (VERSION 3.8)
project ("tether")

set(SDIR Source)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE HEADERS 
    "Headers/*.hpp" "Headers/*.h" 
)

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

set(DEPENDENT_HEADERS)
if (TETHER_VULKAN_API)
    find_package(Vulkan REQUIRED)
    set(DEPENDENT_HEADERS ${DEPENDENT_HEADERS} ${Vulkan_INCLUDE_DIR})

    add_dependencies(${PROJECT_NAME} compileTetherAssets)
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC 
    "Headers"
PRIVATE
    ${DEPENDENT_HEADERS}
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME}
        
    )
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME}
        X11 Xrandr
    )
endif()