cmake_minimum_required(VERSION 3.20)
project(ProjectA VERSION 0.0.1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" STRING FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(Engine)
add_subdirectory(Game)
add_subdirectory(Editor)

# ---------- COPY SHADERS ----------
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders)
set(SHADER_BINARY_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/shaders)

set(
    SHADER_SRC_FILES
    "${SHADER_SOURCE_DIR}/forward.vert"
    "${SHADER_SOURCE_DIR}/forward.frag"
    "${SHADER_SOURCE_DIR}/skybox.vert"
    "${SHADER_SOURCE_DIR}/skybox.frag"
    "${SHADER_SOURCE_DIR}/composition.vert"
    "${SHADER_SOURCE_DIR}/composition.frag"
    "${SHADER_SOURCE_DIR}/shadowmap.vert"
    "${SHADER_SOURCE_DIR}/shadowmap.frag"
)

add_custom_command(
    COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
    OUTPUT ${SHADER_BINARY_DIR}
    COMMENT "Creating ${SHADER_BINARY_DIR}"
)

add_custom_target(Shaders)

foreach(FILE IN LISTS SHADER_SRC_FILES)
    get_filename_component(FILENAME ${FILE} NAME)
    add_custom_command(
        COMMAND
        ${CMAKE_COMMAND} -E copy
        ${FILE}
        ${SHADER_BINARY_DIR}/${FILENAME}
        OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}
        DEPENDS ${FILE}
        COMMENT "Compiling ${FILENAME}"
    )
    target_sources(Shaders PRIVATE ${SHADER_BINARY_DIR}/${FILENAME})
endforeach()

add_dependencies(Engine Shaders)
