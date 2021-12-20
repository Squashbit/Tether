cmake_minimum_required (VERSION 3.8)
project ("TestApp")

file(GLOB_RECURSE SOURCES "Source/*.cpp" "Source/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
    PUBLIC "../../Headers"
    PUBLIC "../../Vendor/SBSTD/SBSTDC++/Headers"
)

target_link_directories(${PROJECT_NAME}
    PRIVATE "../../Build"
)

target_link_libraries(${PROJECT_NAME} 
    ${CMAKE_DL_LIBS} pthread link X11 GL
)
