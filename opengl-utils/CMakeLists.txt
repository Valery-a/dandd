cmake_minimum_required(VERSION 3.10)
project(OpenGL-utils)

# use C++17 (in case std::fs needed)
set(CMAKE_CXX_STANDARD 17)

# autocomplete with YCM & debug with gdb
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)

# shared library
file(GLOB SRC
  "src/glad/glad.c"
  "src/texture/*.cpp"
  "src/shader/*.cpp"
  "src/framebuffer/*.cpp"

  "src/geometries/*.cpp"
  "src/vertexes/*.cpp"
  "src/math/*.cpp"
  "src/render/*.cpp"
  "src/navigation/*.cpp"
)
add_library(opengl_utils SHARED ${SRC})
target_include_directories(opengl_utils PUBLIC 
    include 
    /opt/homebrew/include/glm 
    /usr/local/include/glm
)
