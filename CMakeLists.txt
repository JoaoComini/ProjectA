cmake_minimum_required(VERSION 3.20)
project(ProjectA VERSION 0.0.1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" STRING FORCE)
endif(NOT CMAKE_BUILD_TYPE)

include(FindVulkan)

add_subdirectory(third_party/glm)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/vma)
add_subdirectory(third_party/tinyobjloader)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/**)

add_executable(ProjectA ${SRC_FILES})

target_include_directories(ProjectA PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${Vulkan_INCLUDE_DIR} third_party/stb)

target_link_libraries(ProjectA
    ${Vulkan_LIBRARIES}
    glfw
    glm::glm
    VulkanMemoryAllocator
    tinyobjloader
)

target_compile_definitions(ProjectA PUBLIC
    GLFW_INCLUDE_VULKAN
    GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_FORCE_RADIANS
    STB_IMAGE_IMPLEMENTATION
)

set_target_properties(ProjectA VulkanMemoryAllocator PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

# -------- COMPILE SHADERS --------
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)
get_property(PROJECTA_DIR TARGET ProjectA PROPERTY BINARY_DIR)

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders)
set(SHADER_BINARY_DIR ${PROJECTA_DIR}/resources/shaders)

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

# -------- COPY MODELS --------

file(COPY resources/models DESTINATION ${PROJECTA_DIR}/resources)

add_dependencies(ProjectA Shaders)
