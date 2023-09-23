cmake_minimum_required(VERSION 3.20)
project(Game VERSION 0.0.1)

add_executable(Game src/main.cpp)
target_link_libraries(Game Engine)
set_target_properties(Game PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

# -------- COPY MODELS --------

set(MODELS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources/models)
set(MODELS_OUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/models)

file(GLOB MODEL_SRC_FILES ${MODELS_SRC_DIR}/*)

add_custom_target(Models)

add_custom_command(
    TARGET Models
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MODELS_SRC_DIR} ${MODELS_OUT_DIR}
    DEPENDS ${MODEL_SRC_FILES} ${MODELS_SRC_DIR}
    COMMENT "Copying ${MODELS_SRC_DIR} to ${MODELS_OUT_DIR}"
)

add_dependencies(Game Models)
