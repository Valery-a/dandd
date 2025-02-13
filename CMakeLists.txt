cmake_minimum_required(VERSION 3.10)

project(run)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/tinygltf
)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PRIVATE ${CMAKE_SOURCE_DIR}/external/glad/include)
target_link_libraries(${PROJECT_NAME} glad)
