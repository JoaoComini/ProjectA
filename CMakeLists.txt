cmake_minimum_required(VERSION 3.25)
project(ProjectA VERSION 0.0.1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING FORCE)
endif(NOT CMAKE_BUILD_TYPE)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

include(FindVulkan)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
add_executable(ProjectA ${SRC_FILES})

target_link_libraries(ProjectA ${Vulkan_LIBRARIES} glfw glm::glm)

set_property(TARGET ProjectA PROPERTY CXX_STANDARD 23)
set_property(TARGET ProjectA PROPERTY CXX_STANDARD_REQUIRED ON)
