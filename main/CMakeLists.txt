cmake_minimum_required(VERSION 3.10)

project(pdp11emu-main VERSION 0.1.0 LANGUAGES C)


set(SRCS_PATH "src/*.c")
set(INCLUDE_DIRS "include/")

file(GLOB_RECURSE SRCS ${SRCS_PATH})


set(MAIN "main")

add_executable(${MAIN} ${SRCS})
target_include_directories(${MAIN} PRIVATE ${INCLUDE_DIRS})

set(COMPILE_AND_BUILD_FlAGS
    $<$<CONFIG:Debug>: -Og -g > $<$<CONFIG:Release>: -O3 >
    -W -Wall -Wextra -Wformat $<$<CONFIG:Release>: -Winline >
    -Wno-unused-parameter
    -fsanitize=address)
target_compile_options(${MAIN} PRIVATE ${COMPILE_AND_BUILD_FlAGS})
target_link_options(${MAIN} PRIVATE ${COMPILE_AND_BUILD_FlAGS})


add_subdirectory("../lib/" "${CMAKE_BINARY_DIR}/lib/")
target_link_libraries(${MAIN} lib pthread ncurses)
