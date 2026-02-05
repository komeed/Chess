cmake_minimum_required(VERSION 3.15)
project(Chess C)
set(CMAKE_C_STANDARD 11)

# Find GLFW
find_package(glfw3 CONFIG REQUIRED)

# GLAD
add_library(glad STATIC
        external/glad/src/glad.c
) #hi
target_include_directories(glad PUBLIC
        external/glad/include
        external/
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g -O0")

add_executable(Chess
        src/main.c
        src/graphics/rectangle.c
        src/graphics/rectangle.h
        src/graphics/graphic_libs.h
        src/shader/shader.h
        src/shader/shader.c
        src/utils/files.h
        src/utils/files.c
        src/utils/errorhandler.h
        src/utils/errorhandler.c
        src/utils/util_structs.h
        src/graphics/renderer.h
        src/graphics/renderer.c
        src/graphics/image.h
        src/graphics/image.c
)

# Add Address Sanitizer flags for Debug and RelWithDebInfo configurations
target_compile_options(Chess PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:-fsanitize=address -fno-omit-frame-pointer -g -Wall -Wextra>
)
target_link_options(Chess PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:-fsanitize=address>
)

# Link libraries
target_link_libraries(Chess PRIVATE glfw glad)

# macOS OpenGL
if(APPLE)
    target_link_libraries(Chess PRIVATE "-framework OpenGL")
endif()
