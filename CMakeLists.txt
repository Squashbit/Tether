cmake_minimum_required (VERSION 3.8)
project ("link")

add_subdirectory(Tests/TestApp)

file(GLOB_RECURSE SOURCES 
    "Source/*.cpp" 
    "Source/*.c" 
    "Vendor/SBSTD/SBSTDC++/Source/*.cpp"
    "Vendor/SBSTD/SBSTDC++/Source/*.c"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "Headers"
    PUBLIC "Vendor/SBSTD/SBSTDC++/Headers"
)

target_link_directories(${PROJECT_NAME}
    PRIVATE "Vendor/SBSTD/SBSTDC++/Build"
)
