cmake_minimum_required(VERSION 3.21)
project(modconv LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include all source files within src/
file(GLOB_RECURSE SRC_FILES
	${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Create the executable
add_executable(modconv ${SRC_FILES})

# Include directories
target_include_directories(modconv PRIVATE
    ${PROJECT_SOURCE_DIR}/src/common
    ${PROJECT_SOURCE_DIR}/src/util
    ${PROJECT_SOURCE_DIR}/src
)

# Compiler warnings
if(MSVC)
    target_compile_options(modconv PRIVATE /W4)
else()
    target_compile_options(modconv PRIVATE -Wall -Wextra -Wpedantic)
endif()
