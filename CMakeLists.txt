cmake_minimum_required (VERSION 3.8)
project ("tether")

add_subdirectory(Tests/TestApp)

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" 
    "Source/*.c" 
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
)
