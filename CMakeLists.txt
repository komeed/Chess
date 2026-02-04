cmake_minimum_required(VERSION 3.15)
project(Chess C)
set(CMAKE_C_STANDARD 11)

# Find GLFW (installed via system, brew, vcpkg, etc
find_package(glfw3 CONFIG REQUIRED)

# ---- glad ----
add_library(glad STATIC
        external/glad/src/glad.c
)

target_include_directories(glad PUBLIC
        external/glad/include
)

message(STATUS "GLFW found: ${glfw3_DIR}")

add_executable(Chess
        src/main.c
        src/rectangle.h
)

# Link GLFW
target_link_libraries(Chess PRIVATE glfw)

# ---- macOS OpenGL ----
if (APPLE)
    target_link_libraries(Chess PRIVATE "-framework OpenGL")
endif()