#include "ForwardPass.hpp"

#include "Rendering/Renderer.hpp"

#include "Rendering/RenderGraph/RenderGraphCommand.hpp"

namespace Engine
{
    ForwardPass::ForwardPass(
        RenderContext& renderContext,
        ShaderSource&& vertexSource,
        ShaderSource&& fragmentSource,
        Scene& scene,
        RenderTarget* shadowTarget
    ) : GeometryPass{ renderContext, std::move(vertexSource), std::move(fragmentSource), scene }, shadowTarget{ shadowTarget }
    {
        CreateShadowMapSampler();
    }

    void ForwardPass::RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ForwardPassData& data)
    {
        const auto& shadow = context.Get<ShadowPassData>();

        builder.Read(shadow.shadowmap, {
            .type = RenderTextureAccessType::Binding,
            .binding = {
                .set = 0,
                .location = 6,
                .sampler = RenderTextureSampler::Shadow
            }
        });

        auto extent = GetRenderContext().GetCurrentFrame().GetTarget().GetExtent();

        data.gbuffer = builder.Allocate({
                extent.width,
                extent.height,
                RenderTextureFormat::HDR,
                RenderTextureUsage::RenderTarget | RenderTextureUsage::Sampled,
        });

        builder.Write(data.gbuffer, {
            .type = RenderTextureAccessType::Attachment,
            .attachment = {
                .aspect = RenderTextureAspect::Color,
            }
        });

        context.Add<ForwardPassData>(data);
    }

    void ForwardPass::Render(RenderGraphCommand& command, const ForwardPassData& data)
    {
        command.DrawGeometry(RenderGeometryType::Opaque, "forward");
    }

    void ForwardPass::CreateShadowMapSampler()
    {
        VkSamplerCreateInfo samplerCreateInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        samplerCreateInfo.compareEnable = VK_TRUE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;

        shadowMapSampler = std::make_unique<Vulkan::Sampler>(GetRenderContext().GetDevice(), samplerCreateInfo);
    }

    void ForwardPass::Draw(Vulkan::CommandBuffer& commandBuffer)
    {
        LightsUniform lightsUniform{};
        ShadowUniform shadowUniform{};

        GetMainLightData(lightsUniform, shadowUniform);
        GetAdditionalLightsData(lightsUniform);

        UpdateLightUniform(commandBuffer, lightsUniform);

        BindShadowMap(commandBuffer);

        UpdateShadowUniform(commandBuffer, shadowUniform);

        GeometryPass::Draw(commandBuffer);
    }


    void ForwardPass::GetMainLightData(LightsUniform& lights, ShadowUniform& shadow)
    {
        auto query = scene.Query<Component::Transform, Component::DirectionalLight>();

        auto entity = query.First();

        if (entity == Entity::Null)
        {
            return;
        }

        const auto& [transform, light] = query.GetComponent(entity);

        auto direction = transform.rotation * glm::vec3{ 0, 0, 1 };
        auto view = glm::lookAt(direction, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });

        auto projection = glm::ortho(-25.f, 25.f, -25.f, 25.f, -25.f, 25.f);

        shadow.viewProjection = projection * view;

        lights.lights[0].color = { light.color, light.intensity };
        lights.lights[0].vector = glm::vec4{ direction, 1.f };

        lights.count++;
    }

    void ForwardPass::GetAdditionalLightsData(LightsUniform& uniform)
    {
        auto query = scene.Query<Component::Transform, Component::PointLight>();
        
        for (auto entity : query)
        {
            auto count = uniform.count;

            const auto& [transform, light] = query.GetComponent(entity);
            
            uniform.lights[count].color = { light.color, light.range };
            uniform.lights[count].vector = { transform.position, 0.f };

            uniform.count++;
        }
    }

    void ForwardPass::UpdateLightUniform(Vulkan::CommandBuffer& commandBuffer, LightsUniform uniform)
    {
        auto& frame = GetRenderContext().GetCurrentFrame();

        auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(LightsUniform));

        allocation.SetData(&uniform);

        commandBuffer.BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 5, 0);
    }

    void ForwardPass::BindShadowMap(Vulkan::CommandBuffer& commandBuffer)
    {
        auto& shadowMap = shadowTarget->GetDepthAttachment().GetView();

        commandBuffer.BindImage(shadowMap, *shadowMapSampler, 0, 6, 0);
    }

    void ForwardPass::UpdateShadowUniform(Vulkan::CommandBuffer& commandBuffer, ShadowUniform uniform)
    {
        auto& frame = GetRenderContext().GetCurrentFrame();

        auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(ShadowUniform));

        allocation.SetData(&uniform);

        commandBuffer.BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 7, 0);
    }
};