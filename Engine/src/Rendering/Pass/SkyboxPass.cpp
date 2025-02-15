#include "SkyboxPass.hpp"


#include "Scene/Scene.hpp"
#include "Resource/ResourceManager.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Cubemap.hpp"
#include "Rendering/Mesh.hpp"

#include "Vulkan/ResourceCache.hpp"

namespace Engine
{
    //struct GlobalUniform
    //{
    //    glm::mat4 viewProjection;
    //};

    //SkyboxPass::SkyboxPass(
    //    RenderContext& renderContext,
    //    ShaderSource&& vertexSource,
    //    ShaderSource&& fragmentSource,
    //    Scene& scene
    //) : Pass {renderContext, std::move(vertexSource), std::move(fragmentSource)}, scene(scene)
    //{
    //    cube = Mesh::BuiltIn::Cube(renderContext.GetDevice());
    //}

    //void SkyboxPass::Draw(Vulkan::CommandBuffer& commandBuffer)
    //{
    //    auto query = scene.Query<Component::SkyLight>();

    //    auto entity = query.First();

    //    if (! scene.Valid(entity))
    //    {
    //        return;
    //    }

    //    auto& skyLight = query.GetComponent<Component::SkyLight>(entity);

    //    if (!skyLight.cubemap)
    //    {
    //        return;
    //    }

    //    auto skybox = ResourceManager::Get().LoadResource<Cubemap>(skyLight.cubemap);

    //    auto& resourceCache = GetRenderContext().GetDevice().GetResourceCache();

    //    auto& vertexShader = resourceCache.RequestShader(ShaderStage::Vertex, GetVertexShader(), {});
    //    auto& fragmentShader = resourceCache.RequestShader(ShaderStage::Fragment, GetFragmentShader(), {});

    //    auto& pipelineLayout = GetPipelineLayout({ &vertexShader, &fragmentShader });

    //    commandBuffer.BindPipelineLayout(pipelineLayout);

    //    Vulkan::VertexInputState vertexInputState{};
    //    vertexInputState.attributes.resize(1);
    //    vertexInputState.attributes[0].location = 0;
    //    vertexInputState.attributes[0].binding = 0;
    //    vertexInputState.attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    //    vertexInputState.attributes[0].offset = offsetof(Vertex, position);

    //    vertexInputState.bindings.resize(1);
    //    vertexInputState.bindings[0].binding = 0;
    //    vertexInputState.bindings[0].stride = sizeof(Vertex);
    //    vertexInputState.bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    //    commandBuffer.SetVertexInputState(vertexInputState);

    //    auto [camera, transform] = Renderer::Get().GetMainCamera();

    //    glm::mat4 view = glm::mat3{ glm::inverse(transform) };
    //    auto projection = camera.GetProjection();

    //    GlobalUniform uniform{};
    //    uniform.viewProjection = projection * view;

    //    auto& frame = GetRenderContext().GetCurrentFrame();

    //    auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(GlobalUniform));

    //    allocation.SetData(&uniform);

    //    commandBuffer.BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 0, 0);

    //    commandBuffer.BindImage(skybox->GetImageView(), skybox->GetSampler(), 0, 1, 0);

    //    commandBuffer.FlushDescriptorSet(0);

    //    for (auto& primitive : cube->GetPrimitives())
    //    {
    //        primitive->Draw(commandBuffer);
    //    }
    //}
}