cmake_minimum_required(VERSION 3.20)
project(Engine VERSION 0.0.1)

include(FindVulkan)

add_subdirectory(third_party/glm)
add_subdirectory(third_party/vma)
add_subdirectory(third_party/entt)

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

# -----------------------

# ------- GLSLANG -------

set(BUILD_EXTERNAL OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(ENABLE_HLSL OFF CACHE BOOL "" FORCE)
set(ENABLE_CTEST OFF CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/glslang)

# -------- CEREAL --------

set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/cereal)

# -----------------------

set(ENGINE_SRC_FILES
    "src/Core/Application.cpp"
    "src/Core/Window.cpp"
    "src/Core/WindowSurface.cpp"
    "src/Core/WindowInput.cpp"
    "src/Rendering/Camera.cpp"
    "src/Rendering/Pool/DescriptorPool.cpp"
    "src/Rendering/Pool/SemaphorePool.cpp"
    "src/Rendering/Pool/BufferPool.cpp"
    "src/Rendering/RenderFrame.cpp"
    "src/Rendering/Material.cpp"
    "src/Rendering/Mesh.cpp"
    "src/Rendering/Renderer.cpp"
    "src/Rendering/RenderContext.cpp"
    "src/Rendering/RenderAttachment.cpp"
    "src/Rendering/RenderTarget.cpp"
    "src/Rendering/RenderPipeline.cpp"
    "src/Rendering/Texture.cpp"
    "src/Rendering/Gui.cpp"
    "src/Rendering/Pass/Pass.cpp"
    "src/Rendering/Subpass/Subpass.cpp"
    "src/Rendering/Subpass/GeometrySubpass.cpp"
    "src/Rendering/Subpass/ForwardSubpass.cpp"
    "src/Rendering/Subpass/ShadowSubpass.cpp"
    "src/Rendering/Subpass/CompositionSubpass.cpp"
    "src/Common/FileSystem.cpp"
    "src/Common/Uuid.cpp"
    "src/Vulkan/Buffer.cpp"
    "src/Vulkan/CommandBuffer.cpp"
    "src/Vulkan/CommandPool.cpp"
    "src/Vulkan/DebugMessenger.cpp"
    "src/Vulkan/DescriptorPool.cpp"
    "src/Vulkan/DescriptorSetLayout.cpp"
    "src/Vulkan/Details.cpp"
    "src/Vulkan/Device.cpp"
    "src/Vulkan/Fence.cpp"
    "src/Vulkan/Image.cpp"
    "src/Vulkan/ImageView.cpp"
    "src/Vulkan/Instance.cpp"
    "src/Vulkan/PhysicalDevice.cpp"
    "src/Vulkan/Pipeline.cpp"
    "src/Vulkan/PipelineLayout.cpp"
    "src/Vulkan/PipelineState.cpp"
    "src/Vulkan/Queue.cpp"
    "src/Vulkan/Sampler.cpp"
    "src/Vulkan/Semaphore.cpp"
    "src/Vulkan/ShaderModule.cpp"
    "src/Vulkan/SpirvReflection.cpp"
    "src/Vulkan/GlslCompiler.cpp"
    "src/Vulkan/Surface.cpp"
    "src/Vulkan/Swapchain.cpp"
    "src/Vulkan/Caching/ResourceCache.cpp"
    "src/Scene/Scene.cpp"
    "src/Scene/Entity.cpp"
    "src/Project/Project.cpp"
    "src/Resource/ResourceManager.cpp"
    "src/Resource/ResourceRegistry.cpp"
    "src/Resource/Resource.cpp"
    "src/Resource/Importer/GltfImporter.cpp"
    "src/Resource/Importer/TextureImporter.cpp"
    "src/Resource/Factory/TextureFactory.cpp"
    "src/Resource/Factory/MaterialFactory.cpp"
    "src/Resource/Factory/MeshFactory.cpp"
    
    "src/Resource/Factory/SceneFactory.cpp"
    "src/Resource/Flatbuffers/NativeVertex.cpp"
    "src/Platform/Windows/FileDialog.cpp"
    "src/Rendering/Cubemap.cpp"
    "src/Rendering/Subpass/SkyboxSubpass.cpp"
)

add_library(Engine STATIC ${ENGINE_SRC_FILES})

target_include_directories(Engine
    PUBLIC
    src
    ${Vulkan_INCLUDE_DIR}
)

target_link_libraries(Engine
    ${Vulkan_LIBRARIES}
    glfw
    glm::glm
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
    cereal::cereal
)

target_compile_definitions(Engine PRIVATE
    GLFW_INCLUDE_VULKAN
    GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_FORCE_RADIANS
    TINYGLTF_USE_CPP14
    FLATBUFFERS_USE_STD_SPAN
    CEREAL_SERIALIZE_FUNCTION_NAME=Serialize
    CEREAL_SAVE_FUNCTION_NAME=Save
    CEREAL_LOAD_FUNCTION_NAME=Load
    $<$<PLATFORM_ID:Windows>:PLATFORM_WINDOWS>
    $<$<PLATFORM_ID:Linux>:PLATFORM_LINUX>
)

# - COMPILE FLATBUFFERS -

set(
    FLATBUFFERS_SCHEMAS
    "src/Resource/Flatbuffers/Material.fbs"
    "src/Resource/Flatbuffers/Texture.fbs"
    "src/Resource/Flatbuffers/Mesh.fbs"
)

if (WIN32)
    set(FLATBUFFERS_FLATC_EXECUTABLE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/flatc.exe)
else ()
    set(FLATBUFFERS_FLATC_EXECUTABLE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/flatc)
endif ()

add_custom_target(Engine_Flatbuffers)

foreach(SCHEMA_SRC IN LISTS FLATBUFFERS_SCHEMAS)
    get_filename_component(SCHEMA_DIR ${SCHEMA_SRC} PATH)
    string(REGEX REPLACE "\\.fbs$" "_generated.h" GEN_HEADER ${SCHEMA_SRC})
    add_custom_command(
        COMMAND
        ${FLATBUFFERS_FLATC_EXECUTABLE}
        --cpp --gen-object-api
        -o ${CMAKE_CURRENT_SOURCE_DIR}/${SCHEMA_DIR}
        -I ${CMAKE_CURRENT_SOURCE_DIR}/${SCHEMA_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/${SCHEMA_SRC}
        OUTPUT ${GEN_HEADER}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SCHEMA_SRC}
        COMMENT "flatc generation: ${SCHEMA_SRC}"
    )
    target_sources(Engine_Flatbuffers PRIVATE ${GEN_HEADER})
endforeach()

add_dependencies(Engine Engine_Flatbuffers)

# -------- TESTS --------

add_subdirectory(third_party/catch2)

include(CTest)
include(Catch)

set(ENGINE_TEST_FILES
    "test/Scene/SceneTest.cpp"
    "test/Scene/EntityTest.cpp"
    "test/Scene/MixinTest.cpp"
    "test/Resource/ResourceTest.cpp"
    "test/Resource/Factory/TextureFactoryTest.cpp"
    "test/Resource/Factory/MeshFactoryTest.cpp"
 )

add_executable(Engine_Test ${ENGINE_TEST_FILES})

target_include_directories(Engine_Test
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/test
)

target_link_libraries(Engine_Test PRIVATE Catch2::Catch2WithMain Engine)

catch_discover_tests(Engine_Test)
