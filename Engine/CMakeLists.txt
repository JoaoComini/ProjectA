cmake_minimum_required(VERSION 3.20)
project(Engine VERSION 0.0.1)

include(FindVulkan)

add_subdirectory(third_party/glm)
add_subdirectory(third_party/vma)
add_subdirectory(third_party/entt)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw)

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE INTERNAL "" FORCE)
add_subdirectory(third_party/tinygltf)

set(ENGINE_SRC_FILES
    src/Core/Application.cpp
    src/Core/Window.cpp
    src/Core/WindowSurface.cpp
    src/Rendering/BufferPool.cpp
    src/Rendering/Camera.cpp
    src/Rendering/DescriptorPool.cpp
    src/Rendering/Frame.cpp
    src/Rendering/Material.cpp
    src/Rendering/Mesh.cpp
    src/Rendering/Renderer.cpp
    src/Rendering/SemaphorePool.cpp
    src/Rendering/Target.cpp
    src/Rendering/Texture.cpp
    src/Utils/FileSystem.cpp
    src/Vulkan/Buffer.cpp
    src/Vulkan/CommandBuffer.cpp
    src/Vulkan/CommandPool.cpp
    src/Vulkan/DebugMessenger.cpp
    src/Vulkan/DescriptorPool.cpp
    src/Vulkan/DescriptorSetLayout.cpp
    src/Vulkan/Details.cpp
    src/Vulkan/Device.cpp
    src/Vulkan/Fence.cpp
    src/Vulkan/Framebuffer.cpp
    src/Vulkan/Image.cpp
    src/Vulkan/ImageView.cpp
    src/Vulkan/Instance.cpp
    src/Vulkan/PhysicalDevice.cpp
    src/Vulkan/Pipeline.cpp
    src/Vulkan/PipelineLayout.cpp
    src/Vulkan/Queue.cpp
    src/Vulkan/RenderPass.cpp
    src/Vulkan/Sampler.cpp
    src/Vulkan/Semaphore.cpp
    src/Vulkan/Shader.cpp
    src/Vulkan/ShaderModule.cpp
    src/Vulkan/Surface.cpp
    src/Vulkan/Swapchain.cpp
    "src/Scene/EntityManager.cpp"
    src/Scene/Entity.cpp
    src/Scene/Loader.cpp
 )

add_library(Engine STATIC ${ENGINE_SRC_FILES})

target_include_directories(Engine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIR}
    third_party/stb
)

target_link_libraries(Engine
    ${Vulkan_LIBRARIES}
    glfw
    glm::glm
    VulkanMemoryAllocator
    tinygltf
    EnTT
)

target_compile_definitions(Engine PRIVATE
    GLFW_INCLUDE_VULKAN
    GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_FORCE_RADIANS
    TINYGLTF_USE_CPP14
)

set_target_properties(Engine VulkanMemoryAllocator PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

# -------- TESTS --------

add_subdirectory(third_party/catch2)

include(CTest)
include(Catch)

set(ENGINE_TEST_FILES test/Scene/SceneTest.cpp test/Scene/EntityTest.cpp)

add_executable(tests ${ENGINE_TEST_FILES})

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain Engine)

set_target_properties(tests PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

catch_discover_tests(tests)

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

