cmake_minimum_required(VERSION 3.16)
project(huilin-jian VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 核心库
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