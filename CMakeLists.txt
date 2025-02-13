# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.16)

project(hellgate)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(balltze-lib/balltze.cmake)

file(GLOB PURE_DOOM_SRC puredoom/src/DOOM/*.*)
include_directories(puredoom/src/)

add_definitions(
    -DDOOM_IMPLEMENT_PRINT
    -DDOOM_IMPLEMENT_MALLOC
    -DDOOM_IMPLEMENT_FILE_IO
    -DDOOM_IMPLEMENT_SOCKETS
    -DDOOM_IMPLEMENT_GETTIME
    -DDOOM_IMPLEMENT_EXIT
    -DDOOM_IMPLEMENT_GETENV
)

add_library(${PROJECT_NAME} MODULE
    src/hellgate.cpp
    src/main.cpp
    ${PURE_DOOM_SRC}
)

target_link_libraries(${PROJECT_NAME} balltze)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "hellgate")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static -static-libgcc -static-libstdc++")
