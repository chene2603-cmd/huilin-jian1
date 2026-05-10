cmake_minimum_required(VERSION 3.16)
project(huilin-jian VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 严格编译，提前暴露潜在问题
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 构建模式
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 引擎库
add_library(engine
    engine/engine.c
)

target_include_directories(engine PUBLIC
    ${CMAKE_SOURCE_DIR}
)

# 主程序
add_executable(huilin
    main.c
)

target_link_libraries(huilin PRIVATE engine)