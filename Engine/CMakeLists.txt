cmake_minimum_required(VERSION 3.20)
project(Engine VERSION 0.0.1)

include(FindVulkan)

add_subdirectory(third_party/vma)
add_subdirectory(third_party/entt)

# ------- GLM -------

set(GLM_ENABLE_CXX_20 ON CACHE BOOL "")
add_subdirectory(third_party/glm)

# ----- YAML-CPP -----

set(YAML_CPP_INSTALL OFF)
set(YAML_CPP_BUILD_TOOLS OFF)
add_subdirectory(third_party/yaml-cpp)

# ------- GLFW --------

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw)

# ----- TINYGLTF ------

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE INTERNAL "" FORCE)
add_subdirectory(third_party/tinygltf)

# -------- STB --------

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE third_party/stb)

# -------- IMGUI -------

set(IMGUI_SRC_FILES
    "third_party/imgui/imgui.cpp"
    "third_party/imgui/imgui_demo.cpp"
    "third_party/imgui/imgui_draw.cpp"
    "third_party/imgui/imgui_widgets.cpp"
    "third_party/imgui/imgui_tables.cpp"
    "third_party/imgui/backends/imgui_impl_vulkan.cpp"
    "third_party/imgui/backends/imgui_impl_glfw.cpp"
    "third_party/imgui/misc/cpp/imgui_stdlib.cpp"
)

add_library(ImGui STATIC ${IMGUI_SRC_FILES})

target_include_directories(ImGui
    PUBLIC
    third_party/imgui
    ${Vulkan_INCLUDE_DIR}
)

target_compile_definitions(ImGui PRIVATE GLFW_INCLUDE_NONE)

target_link_libraries(ImGui
    ${Vulkan_LIBRARIES}
    glfw
)

# ------ IMGUIZMO -------

add_library(ImGuizmo STATIC "third_party/imguizmo/ImGuizmo.cpp")

target_include_directories(ImGuizmo
    PUBLIC
    third_party/imguizmo
)

target_link_libraries(ImGuizmo ImGui)

# ------ FLATBUFFERS -----

set(FLATBUFFERS_BUILD_TESTS OFF)
add_subdirectory(third_party/flatbuffers)

# ------ SPIRV-CROSS -----

set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_C_API OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/spirv-cross)

# ------- GLSLANG -------

set(BUILD_EXTERNAL OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(ENABLE_HLSL OFF CACHE BOOL "" FORCE)
set(ENABLE_CTEST OFF CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/glslang)

# -------- CEREAL --------

set(BUILD_SANDBOX OFF CACHE BOOL "" FORCE)
set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/cereal)

# --------- LUA ----------

add_library(lua
  "third_party/lua/lapi.c"
  "third_party/lua/lauxlib.c"
  "third_party/lua/lbaselib.c"
  "third_party/lua/lcode.c"
  "third_party/lua/lcorolib.c"
  "third_party/lua/lctype.c"
  "third_party/lua/ldblib.c"
  "third_party/lua/ldebug.c"
  "third_party/lua/ldo.c"
  "third_party/lua/ldump.c"
  "third_party/lua/lfunc.c"
  "third_party/lua/lgc.c"
  "third_party/lua/linit.c"
  "third_party/lua/liolib.c"
  "third_party/lua/llex.c"
  "third_party/lua/lmathlib.c"
  "third_party/lua/lmem.c"
  "third_party/lua/loadlib.c"
  "third_party/lua/lobject.c"
  "third_party/lua/lopcodes.c"
  "third_party/lua/loslib.c"
  "third_party/lua/lparser.c"
  "third_party/lua/lstate.c"
  "third_party/lua/lstring.c"
  "third_party/lua/lstrlib.c"
  "third_party/lua/ltable.c"
  "third_party/lua/ltablib.c"
  "third_party/lua/ltests.c"
  "third_party/lua/ltm.c"
  "third_party/lua/lua.c"
  "third_party/lua/lundump.c"
  "third_party/lua/lutf8lib.c"
  "third_party/lua/lvm.c"
  "third_party/lua/lzio.c"
)

add_library(lua::lua ALIAS lua)

if (UNIX)
  target_compile_definitions(lua PRIVATE LUA_USE_LINUX)
  target_link_libraries(lua PRIVATE dl)
endif()

target_include_directories(
    lua
    PUBLIC
    third_party/lua
)

# -------- SOL2 --------

set(SOL2_LUA_VERSION "5.4.7" CACHE STRING "The version of Lua used")
set(SOL2_BUILD_LUA FALSE CACHE BOOL "")
add_subdirectory(third_party/sol2)

# -------- JOLT ---------

set(OVERRIDE_CXX_FLAGS FALSE CACHE BOOL "")
add_subdirectory(third_party/JoltPhysics/Build)

# ----- EMBED SHADERS ------

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/static/shaders)

set(
    SHADER_SRC_FILES
    "${SHADER_SOURCE_DIR}/forward.vert.glsl"
    "${SHADER_SOURCE_DIR}/forward.frag.glsl"
    "${SHADER_SOURCE_DIR}/skybox.vert.glsl"
    "${SHADER_SOURCE_DIR}/skybox.frag.glsl"
    "${SHADER_SOURCE_DIR}/composition.vert.glsl"
    "${SHADER_SOURCE_DIR}/composition.frag.glsl"
    "${SHADER_SOURCE_DIR}/shadowmap.vert.glsl"
    "${SHADER_SOURCE_DIR}/shadowmap.frag.glsl"
    "${SHADER_SOURCE_DIR}/imgui.vert.glsl"
    "${SHADER_SOURCE_DIR}/imgui.frag.glsl"
)

embed_static(Shaders ${SHADER_SRC_FILES})

# ---------------------------

file(GLOB_RECURSE ENGINE_SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

add_library(Engine STATIC ${ENGINE_SRC_FILES}
        src/Rendering/RenderTexture.cpp
        src/Rendering/RenderTexture.hpp
        src/Rendering/RenderBuffer.hpp)

target_include_directories(Engine
    PUBLIC
    src
    ${Vulkan_INCLUDE_DIR}
)

target_link_libraries(Engine
    PUBLIC
    ${Vulkan_LIBRARIES}
    glfw
    glm
    VulkanMemoryAllocator
    stb
    tinygltf
    EnTT
    ImGui
    ImGuizmo
    yaml-cpp
    flatbuffers
    spirv-cross-cpp
    spirv-cross-glsl
    glslang
    glslang-default-resource-limits
    SPIRV
    cereal
    lua
    sol2
    Jolt
    Shaders
)

target_compile_definitions(Engine 
    PRIVATE
    GLFW_INCLUDE_VULKAN
    TINYGLTF_USE_CPP14
    FLATBUFFERS_USE_STD_SPAN
    CEREAL_SERIALIZE_FUNCTION_NAME=Serialize
    CEREAL_SAVE_FUNCTION_NAME=Save
    CEREAL_LOAD_FUNCTION_NAME=Load
    SOL_ALL_SAFETIES_ON
    SOL_DEFAULT_PASS_ON_ERROR
    $<$<PLATFORM_ID:Windows>:PLATFORM_WINDOWS>
    $<$<PLATFORM_ID:Linux>:PLATFORM_LINUX>
)

target_precompile_headers(Engine
    PUBLIC
    "src/Precompiled.hpp"
)

# - COMPILE FLATBUFFERS -

set(
    FLATBUFFERS_SCHEMAS
    "src/Resource/Flatbuffers/Material.fbs"
    "src/Resource/Flatbuffers/Texture.fbs"
    "src/Resource/Flatbuffers/Mesh.fbs"
)

add_custom_target(Engine_Flatbuffers ALL)

foreach(SCHEMA_SRC IN LISTS FLATBUFFERS_SCHEMAS)
    get_filename_component(SCHEMA_DIR ${SCHEMA_SRC} PATH)
    string(REGEX REPLACE "\\.fbs$" "_generated.h" GEN_HEADER ${SCHEMA_SRC})
    add_custom_command(
        COMMAND
        flatc
        --cpp --gen-object-api
        -o ${CMAKE_CURRENT_SOURCE_DIR}/${SCHEMA_DIR}
        -I ${CMAKE_CURRENT_SOURCE_DIR}/${SCHEMA_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/${SCHEMA_SRC}
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${GEN_HEADER}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SCHEMA_SRC}
        COMMENT "flatc generation: ${SCHEMA_SRC}"
    )
    target_sources(Engine_Flatbuffers PRIVATE ${GEN_HEADER})
endforeach()

add_dependencies(Engine_Flatbuffers flatc)

add_dependencies(Engine Engine_Flatbuffers)

# -------- TESTS --------

add_subdirectory(third_party/catch2)

include(Catch)

set(ENGINE_TEST_FILES
    "test/Scene/SceneTest.cpp"
    
    "test/Resource/ResourceTest.cpp"
    "test/Resource/Factory/TextureFactoryTest.cpp"
    "test/Resource/Factory/MeshFactoryTest.cpp"
  "test/Rendering/RenderGraph/RenderGraphTest.cpp")

add_executable(Engine_Test ${ENGINE_TEST_FILES})

target_include_directories(Engine_Test
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/test
)

target_link_libraries(Engine_Test PRIVATE Catch2::Catch2WithMain Engine)

catch_discover_tests(Engine_Test)
