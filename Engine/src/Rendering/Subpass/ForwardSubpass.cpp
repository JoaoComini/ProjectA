#include "ForwardSubpass.hpp"


namespace Engine
{
    ForwardSubpass::ForwardSubpass(
        Vulkan::Device& device,
        Vulkan::ShaderSource&& vertexSource,
        Vulkan::ShaderSource&& fragmentSource,
        Scene& scene,
        Camera& shadowCamera,
        std::vector<std::unique_ptr<RenderTarget>>& shadowRenderTargets
    ) : GeometrySubpass { device, std::move(vertexSource), std::move(fragmentSource), scene },
        shadowCamera{ shadowCamera },
        shadowRenderTargets{ shadowRenderTargets }
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

        shadowMapSampler = std::make_unique<Vulkan::Sampler>(device, samplerCreateInfo);
    }

    void ForwardSubpass::Draw(Vulkan::CommandBuffer& commandBuffer)
    {
        LightUniform lightUniform{};
        ShadowUniform shadowUniform{};

        auto [entity, found] = scene.FindFirstEntity<Component::Transform, Component::DirectionalLight>();

        if (found)
        {
            const auto& transform = entity.GetComponent<Component::Transform>();

            auto projection = shadowCamera.GetProjection();
            auto view = glm::inverse(transform.GetLocalMatrix());

            shadowUniform.viewProjection = projection * view;

            const auto& light = entity.GetComponent<Component::DirectionalLight>();

            lightUniform.color = { light.color, light.intensity };
            lightUniform.vector = glm::vec4{ transform.rotation * glm::vec3{ 0.f, 0.f, 1.f }, 1.f };
        }

        UpdateLightUniform(commandBuffer, lightUniform);

        BindShadowMap();

        UpdateShadowUniform(commandBuffer, shadowUniform);

        GeometrySubpass::Draw(commandBuffer);
    }

    void ForwardSubpass::UpdateLightUniform(Vulkan::CommandBuffer& commandBuffer, LightUniform uniform)
    {
        auto& frame = Renderer::Get().GetCurrentFrame();

        auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(LightUniform));

        allocation.SetData(&uniform);

        BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 4, 0);
    }

    void ForwardSubpass::BindShadowMap()
    {
        auto& shadowTarget = shadowRenderTargets[Renderer::Get().GetCurrentFrameIndex()];
        auto& shadowMap = shadowTarget->GetViews()[0];

        BindImage(*shadowMap, *shadowMapSampler, 0, 5, 0);
    }

    void ForwardSubpass::UpdateShadowUniform(Vulkan::CommandBuffer& commandBuffer, ShadowUniform uniform)
    {
        auto& frame = Renderer::Get().GetCurrentFrame();

        auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(ShadowUniform));

        allocation.SetData(&uniform);

        BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 6, 0);
    }
};