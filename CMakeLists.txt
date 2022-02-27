cmake_minimum_required (VERSION 3.8)
project ("tether")

add_subdirectory(Tests/SimpleWindow)
add_subdirectory(Tests/MultipleWindows)

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" "Source/*.c" 
    "Headers/*.hpp" "Headers/*.h" 
)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DOPENGL_API -DTETHER_PREVIEW_FEATURES)
add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
)
