cmake_minimum_required(VERSION 3.20)
project(Game VERSION 0.0.1)

set(GAME_SRC_FILES)
    
add_executable(Game "src/ProjectA.cpp" ${GAME_SRC_FILES})

target_include_directories(Game
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Game Engine)
set_target_properties(Game PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)