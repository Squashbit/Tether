cmake_minimum_required (VERSION 3.8)
project ("tether")

set(SDIR Source)
set(CMAKE_CXX_STANDARD 17)

option(TETHER_VULKAN_API "Build tether with Vulkan capabilities" ON)
option(TETHER_OPENGL_API "Build tether with OpenGL capabilities" OFF)
option(TETHER_PREVIEW_FEATURES "Build tether with preview features" OFF)

add_subdirectory(Tests/SimpleWindow)
add_subdirectory(Tests/RendererTesting)
add_subdirectory(Tests/MultipleWindows)

file(GLOB_RECURSE HEADERS 
    "Headers/*.hpp" "Headers/*.h" 
)

set(DEPENDENT_SOURCES)
if (TETHER_VULKAN_API)
    set(DEPENDENT_SOURCES ${DEPENDENT_SOURCES} )
elseif (TETHER_PREVIEW_FEATURES)
    set(DEPENDENT_SOURCES ${DEPENDENT_SOURCES} 
        ${SDIR}/Window.cpp
    )
endif()

add_library(${PROJECT_NAME} STATIC 
    ${HEADERS}

    ${SDIR}/Common/Color.cpp ${SDIR}/Common/IDisposable.cpp 
    ${SDIR}/Common/StringUtils.cpp ${SDIR}/Controls/Control.cpp
    ${SDIR}/Devices/DeviceManager.cpp ${SDIR}/Devices/Monitor.cpp
    ${SDIR}/Events/EventHandler.cpp ${SDIR}/Events/WindowErrorEvent.cpp
    ${SDIR}/Events/WindowMoveEvent.cpp ${SDIR}/Events/WindowResizeEvent.cpp
    ${SDIR}/Input/InputListener.cpp ${SDIR}/Input/KeyCharInfo.cpp 
    ${SDIR}/Input/KeyInfo.cpp ${SDIR}/Input/MouseMoveInfo.cpp 
    ${SDIR}/Input/RawMouseMoveInfo.cpp ${SDIR}/Application.cpp ${SDIR}/IWindow.cpp 
    ${SDIR}/Renderer/RenderContext.cpp

    ${DEPENDENT_SOURCES})

if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SDIR}/Devices/Win32_DeviceManager.cpp
        ${SDIR}/Win32_Application.cpp
        ${SDIR}/Win32_IWindow.cpp
    )
elseif (UNIX)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SDIR}/Devices/linux_DeviceManager.cpp
        ${SDIR}/X11_Application.cpp
        ${SDIR}/X11_IWindow.cpp
    )
endif()

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
)
