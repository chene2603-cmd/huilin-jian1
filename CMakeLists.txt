
cmake_minimum_required(VERSION 3.16)
project(huilin-jian VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# GLFW
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# GLEW
include(FetchContent)
FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/nigels-com/glew.git
    GIT_TAG glew-2.2.0
)
FetchContent_MakeAvailable(glew)

# 引擎库
add_library(engine
    engine/engine.c
    engine/renderer.c
    core/math.c
    core/camera.c
)

target_include_directories(engine PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${glew_SOURCE_DIR}/include
)

target_link_libraries(engine PUBLIC
    glfw
    ${CMAKE_DL_LIBS}
)

# 在Unix系统上链接额外的库
if(UNIX AND NOT APPLE)
    target_link_libraries(engine PUBLIC
        GL
        X11
        pthread
    )
endif()

# 主程序
add_executable(huilin
    main.c
)

target_link_libraries(huilin PRIVATE engine)
