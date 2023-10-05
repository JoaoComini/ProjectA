cmake_minimum_required(VERSION 3.20)
project(ProjectA VERSION 0.0.1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" STRING FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

add_subdirectory(Engine)
add_subdirectory(Game)
add_subdirectory(Editor)

# -------- COPY MODELS --------

set(MODELS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources/models)
set(MODELS_OUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/models)

file(GLOB MODEL_SRC_FILES ${MODELS_SRC_DIR}/*)

add_custom_target(Models)

add_custom_command(
    TARGET Models
    COMMAND ${CMAKE_COMMAND} -E echo "Copying ${MODELS_SRC_DIR} to ${MODELS_OUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MODELS_SRC_DIR} ${MODELS_OUT_DIR}
    DEPENDS ${MODEL_SRC_FILES} ${MODELS_SRC_DIR}
)

add_dependencies(Game Models)
add_dependencies(Editor Models)

# -------- COMPILE SHADERS --------
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders)
set(SHADER_BINARY_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/shaders)

file(GLOB SHADERS_SRC_FILES ${SHADER_SOURCE_DIR}/*)

add_custom_command(
    COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
    OUTPUT ${SHADER_BINARY_DIR}
    COMMENT "Creating ${SHADER_BINARY_DIR}"
)

foreach(FILE IN LISTS SHADERS_SRC_FILES)
    get_filename_component(FILENAME ${FILE} NAME)
    add_custom_command(
        COMMAND
        ${glslc_executable}
        -o ${SHADER_BINARY_DIR}/${FILENAME}.spv
        ${FILE}
        OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}.spv
        DEPENDS ${FILE} ${SHADER_BINARY_DIR}
        COMMENT "Compiling ${FILENAME}"
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
endforeach()

add_custom_target(Shaders ALL DEPENDS ${SPV_SHADERS})

add_dependencies(Engine Shaders)
