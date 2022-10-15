set(APP_NAME swap-interval-test)

cmake_minimum_required(VERSION 3.23)
project(${APP_NAME})

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

include(FetchContent)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 7482de6071d21db77a7236155da44c172a7f6c9e
)

FetchContent_Declare(
        glew
        GIT_REPOSITORY https://github.com/perlmint/glew-cmake.git
        GIT_TAG f456deace7b408655109aaeff71421ef2d3858c6
)

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(glew)

add_executable(${APP_NAME} main.cpp)
target_link_libraries(${APP_NAME} PRIVATE OpenGL::GL glfw libglew_static)
