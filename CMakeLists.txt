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

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

add_library(engine
    engine/engine.c
    core/math.c
    core/camera.c
)

target_include_directories(engine PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(engine PUBLIC glfw)

add_executable(huilin
    main.c
)

target_link_libraries(huilin PRIVATE engine)
