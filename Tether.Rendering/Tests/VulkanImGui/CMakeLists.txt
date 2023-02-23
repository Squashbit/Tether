cmake_minimum_required (VERSION 3.8)
project ("VulkanImGui")

set(IMGUI_DIR ${TETHER_PROJECT_DIR}/Tether.Rendering/Vendor/ImGui)
set (SOURCES Source/Main.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)

if (WIN32 AND ("${CMAKE_BUILD_TYPE}" MATCHES "Release"))
    add_executable(
        ${PROJECT_NAME} 
        WIN32 
        ${SOURCES} 
    )
else()
    add_executable(
        ${PROJECT_NAME} 
        ${SOURCES} 
    )
endif()
add_dependencies(${PROJECT_NAME} tether tetherRendering VulkanMemoryAllocator)

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} 
PUBLIC 
	"Headers"
	${TETHER_PROJECT_DIR}/Tether/Headers
	${TETHER_PROJECT_DIR}/Tether.Rendering/Headers
PRIVATE
	${TETHER_PROJECT_DIR}/Tether.Rendering/Vendor/
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
	
