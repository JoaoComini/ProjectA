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

# ------ FLATBUFFERS -----

set(FLATBUFFERS_BUILD_TESTS OFF)
add_subdirectory(third_party/flatbuffers)

# -----------------------

set(ENGINE_SRC_FILES
    "src/Core/Application.cpp"
    "src/Core/RenderSystem.cpp"
    "src/Core/Window.cpp"
    "src/Core/WindowSurface.cpp"
    "src/Core/WindowInput.cpp"
    
    "src/Rendering/BufferPool.cpp"
    "src/Rendering/Camera.cpp"
    "src/Rendering/DescriptorPool.cpp"
    "src/Rendering/Frame.cpp"
    "src/Rendering/Material.cpp"
    "src/Rendering/Mesh.cpp"
    "src/Rendering/Renderer.cpp"
    "src/Rendering/SemaphorePool.cpp"
    "src/Rendering/Target.cpp"
    "src/Rendering/Texture.cpp"
    "src/Rendering/Gui.cpp"
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
    "src/Vulkan/Framebuffer.cpp"
    "src/Vulkan/Image.cpp"
    "src/Vulkan/ImageView.cpp"
    "src/Vulkan/Instance.cpp"
    "src/Vulkan/PhysicalDevice.cpp"
    "src/Vulkan/Pipeline.cpp"
    "src/Vulkan/PipelineLayout.cpp"
    "src/Vulkan/Queue.cpp"
    "src/Vulkan/RenderPass.cpp"
    "src/Vulkan/Sampler.cpp"
    "src/Vulkan/Semaphore.cpp"
    "src/Vulkan/Shader.cpp"
    "src/Vulkan/ShaderModule.cpp"
    "src/Vulkan/Surface.cpp"
    "src/Vulkan/Swapchain.cpp"
    "src/Scene/Scene.cpp"
    "src/Scene/Entity.cpp"
    "src/Scene/SceneSerializer.cpp"
    "src/Project/Project.cpp"
    "src/Resource/ResourceManager.cpp"
    "src/Resource/Resource.cpp"
    "src/Resource/Importer/GltfImporter.cpp"
    "src/Resource/Factory/TextureFactory.cpp"
    "src/Resource/Factory/MaterialFactory.cpp"
    "src/Resource/Factory/MeshFactory.cpp"
    "src/Resource/Prefab.cpp"
    "src/Resource/Factory/PrefabFactory.cpp"
    "src/Resource/Importer/TextureImporter.cpp"
    "src/Platform/Windows/FileDialog.cpp"
 "src/Resource/Flatbuffers/NativeVertex.cpp" "src/Resource/ResourceRegistry.cpp")

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
    yaml-cpp
    flatbuffers
)

target_compile_definitions(Engine PRIVATE
    GLFW_INCLUDE_VULKAN
    GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_FORCE_RADIANS
    TINYGLTF_USE_CPP14
    FLATBUFFERS_USE_STD_SPAN
    $<$<PLATFORM_ID:Windows>:PLATFORM_WINDOWS>
    $<$<PLATFORM_ID:Linux>:PLATFORM_LINUX>
)

# - COMPILE FLATBUFFERS -

set(
    FLATBUFFERS_SRC_FILES
    "src/Resource/Flatbuffers/Material.fbs"
    "src/Resource/Flatbuffers/Texture.fbs"
    "src/Resource/Flatbuffers/Mesh.fbs"
    "src/Resource/Flatbuffers/Prefab.fbs"
)

if (WIN32)
    set(FLATBUFFERS_FLATC_EXECUTABLE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/flatc.exe)
else ()
    set(FLATBUFFERS_FLATC_EXECUTABLE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/flatc)
endif ()

foreach(FILE IN LISTS FLATBUFFERS_SRC_FILES)
    get_filename_component(FLATBUFFERS_SRC_DIR ${FILE} PATH)
    get_filename_component(FILENAME ${FILE} NAME_WE)
    add_custom_command(
        COMMAND
        ${FLATBUFFERS_FLATC_EXECUTABLE}
        --cpp --gen-object-api
        -o ${CMAKE_CURRENT_SOURCE_DIR}/${FLATBUFFERS_SRC_DIR}
        -I ${CMAKE_CURRENT_SOURCE_DIR}/${FLATBUFFERS_SRC_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
        OUTPUT ${FLATBUFFERS_SRC_DIR}/${FILENAME}_generated.h
        DEPENDS ${FILE} ${FLATBUFFERS_SRC_DIR} flatc
        COMMENT "flatc generation: ${FILE}"
    )
    list(APPEND FLATBUFFERS_GEN ${FLATBUFFERS_SRC_DIR}/${FILENAME}_generated.h)
endforeach()

add_custom_target(Engine_Flatbuffers DEPENDS ${FLATBUFFERS_GEN})

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
    "test/Resource/Factory/PrefabFactoryTest.cpp"
)

add_executable(Engine_Test ${ENGINE_TEST_FILES})

target_include_directories(Engine_Test
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/test
)

target_link_libraries(Engine_Test PRIVATE Catch2::Catch2WithMain Engine)

catch_discover_tests(Engine_Test)
