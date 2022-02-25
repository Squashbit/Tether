cmake_minimum_required (VERSION 3.8)
project ("MultipleWindows")

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" "Source/*.c"
    "Headers/*.hpp" "Source/*.h"
)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DTETHER_PREVIEW_FEATURES)
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
    PUBLIC "../../Headers"
)

target_link_directories(${PROJECT_NAME}
    PRIVATE "../../Build"
)

target_link_libraries(${PROJECT_NAME} 
    ${CMAKE_DL_LIBS} pthread tether X11
)
