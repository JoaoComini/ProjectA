#include "ForwardSubpass.hpp"

#include "Rendering/Renderer.hpp"

namespace Engine
{
    ForwardSubpass::ForwardSubpass(
        RenderContext& renderContext,
        Vulkan::ShaderSource&& vertexSource,
        Vulkan::ShaderSource&& fragmentSource,
        Scene& scene,
        RenderTarget* shadowTarget
    ) : GeometrySubpass{ renderContext, std::move(vertexSource), std::move(fragmentSource), scene }, shadowTarget{ shadowTarget }
    {
        CreateShadowMapSampler();
    }

    void ForwardSubpass::CreateShadowMapSampler()
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

    void ForwardSubpass::Draw(Vulkan::CommandBuffer& commandBuffer)
    {
        LightsUniform lightsUniform{};
        ShadowUniform shadowUniform{};

        auto entity = scene.FindFirstEntity<Component::Transform, Component::DirectionalLight>();

        size_t count = 0;
        if (entity)
        {
            const auto& transform = entity.GetComponent<Component::Transform>();

            auto direction = transform.rotation * glm::vec3{ 0, 0, 1 };
            auto view = glm::lookAt(direction, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });

            auto projection = glm::ortho(-25.f, 25.f, -25.f, 25.f, -25.f, 25.f);

            shadowUniform.viewProjection = projection * view;

            const auto& light = entity.GetComponent<Component::DirectionalLight>();

            lightsUniform.lights[count].color = { light.color, light.intensity };
            lightsUniform.lights[count].vector = glm::vec4{ direction, 1.f };

            count++;
        }

        scene.ForEachEntity<Component::Transform, Component::PointLight>([&](Entity entity) {
            const auto& transform = entity.GetComponent<Component::Transform>();
            const auto& light = entity.GetComponent<Component::PointLight>();

            lightsUniform.lights[count].color = { light.color, light.range };
            lightsUniform.lights[count].vector = { transform.position, 0.f };

            count++;
        });

        lightsUniform.count = count;

        UpdateLightUniform(commandBuffer, lightsUniform);

        BindShadowMap();

        UpdateShadowUniform(commandBuffer, shadowUniform);

        GeometrySubpass::Draw(commandBuffer);
    }

    void ForwardSubpass::UpdateLightUniform(Vulkan::CommandBuffer& commandBuffer, LightsUniform uniform)
    {
        auto& frame = GetRenderContext().GetCurrentFrame();

        auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(LightsUniform));

        allocation.SetData(&uniform);

        BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 5, 0);
    }

    void ForwardSubpass::BindShadowMap()
    {
        auto& shadowMap = shadowTarget->GetDepthAttachment()->GetView();

        BindImage(shadowMap, *shadowMapSampler, 0, 6, 0);
    }

    void ForwardSubpass::UpdateShadowUniform(Vulkan::CommandBuffer& commandBuffer, ShadowUniform uniform)
    {
        auto& frame = GetRenderContext().GetCurrentFrame();

        auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(ShadowUniform));

        allocation.SetData(&uniform);

        BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 7, 0);
    }
};