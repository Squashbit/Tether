cmake_minimum_required (VERSION 3.8)
project ("tether")

set(SDIR Source)
set(CMAKE_CXX_STANDARD 17)

option(TETHER_VULKAN_API "Build tether with Vulkan capabilities" OFF)
option(TETHER_OPENGL_API "Build tether with OpenGL capabilities" OFF)
option(TETHER_PREVIEW_FEATURES "Build tether with preview features" OFF)
option(TETHER_BUILD_TESTS "Build tether with tests" OFF)

message(STATUS "TETHER_VULKAN_API = ${TETHER_VULKAN_API}")
message(STATUS "TETHER_OPENGL_API = ${TETHER_OPENGL_API}")
message(STATUS "TETHER_PREVIEW_FEATURES = ${TETHER_PREVIEW_FEATURES}")
message(STATUS "TETHER_BUILD_TESTS = ${TETHER_BUILD_TESTS}")

if (TETHER_BUILD_TESTS)
    add_subdirectory(Tests/SimpleWindow)
    add_subdirectory(Tests/RendererTesting)
    add_subdirectory(Tests/MultipleWindows)
endif()

file(GLOB_RECURSE HEADERS 
    "Headers/*.hpp" "Headers/*.h" 
)

add_library(${PROJECT_NAME} STATIC 
    ${HEADERS}

    ${SDIR}/Common/Color.cpp ${SDIR}/Common/IDisposable.cpp 
    ${SDIR}/Common/StringUtils.cpp ${SDIR}/Controls/Control.cpp
    ${SDIR}/Devices/DeviceManager.cpp ${SDIR}/Devices/Monitor.cpp
    ${SDIR}/Events/EventHandler.cpp ${SDIR}/Events/WindowErrorEvent.cpp
    ${SDIR}/Events/WindowMoveEvent.cpp ${SDIR}/Events/WindowResizeEvent.cpp
    ${SDIR}/Input/InputListener.cpp ${SDIR}/Input/KeyCharInfo.cpp 
    ${SDIR}/Input/KeyInfo.cpp ${SDIR}/Input/MouseMoveInfo.cpp 
    ${SDIR}/Input/RawMouseMoveInfo.cpp ${SDIR}/Application.cpp 
    ${SDIR}/SimpleWindow.cpp
)

set(DEPENDENT_SOURCES)
if (TETHER_VULKAN_API)
    set(DEPENDENT_SOURCES ${DEPENDENT_SOURCES} 
        ${SDIR}/Renderer/Vulkan/Device.cpp
        ${SDIR}/Renderer/Vulkan/DeviceLoader.cpp
        ${SDIR}/Renderer/Vulkan/Instance.cpp
        ${SDIR}/Renderer/Vulkan/InstanceLoader.cpp
        ${SDIR}/Renderer/Vulkan/SimpleNative.cpp
        ${SDIR}/Renderer/Vulkan/AdvancedNative.cpp
        ${SDIR}/Renderer/Vulkan/Surface.cpp
        ${SDIR}/Renderer/Vulkan/Swapchain.cpp
        
        ${SDIR}/Renderer/Vulkan/VkUtils.cpp
        ${SDIR}/Vulkan_Application.cpp
    )
endif()
if (TETHER_PREVIEW_FEATURES)
    set(DEPENDENT_SOURCES ${DEPENDENT_SOURCES} 
        ${SDIR}/GraphicalWindow.cpp ${SDIR}/Renderer/RenderContext.cpp
    )
endif()

# Set the sources
if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SDIR}/Devices/Win32_DeviceManager.cpp
        ${SDIR}/Win32_Application.cpp
        ${SDIR}/Win32_SimpleWindow.cpp

        ${DEPENDENT_SOURCES}
    )
elseif (UNIX)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SDIR}/Devices/linux_DeviceManager.cpp
        ${SDIR}/X11_Application.cpp
        ${SDIR}/X11_SimpleWindow.cpp

        ${DEPENDENT_SOURCES}
    )
endif()

set(DEPENDENT_HEADERS)
if (TETHER_VULKAN_API)
    find_package(Vulkan REQUIRED)
    set(DEPENDENT_HEADERS ${DEPENDENT_HEADERS} ${Vulkan_INCLUDE_DIR})
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC 
    "Headers"
PRIVATE
    ${DEPENDENT_HEADERS}
)
