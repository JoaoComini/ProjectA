#include "ForwardSubpass.hpp"

#include "Rendering/Renderer.hpp"

namespace Engine
{
    ForwardSubpass::ForwardSubpass(
        RenderContext& renderContext,
        Vulkan::ShaderSource&& vertexSource,
        Vulkan::ShaderSource&& fragmentSource,
        Scene& scene,
        Camera& shadowCamera,
        RenderTarget* shadowTarget
    ) : GeometrySubpass { renderContext, std::move(vertexSource), std::move(fragmentSource), scene },
        shadowCamera{ shadowCamera },
        shadowTarget{ shadowTarget }
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

        auto& directionalLights = Renderer::Get().GetDirectionalLights();

        size_t count = 0;
        for (auto& [_, light] : directionalLights)
        {
            auto projection = shadowCamera.GetProjection();
            auto view = glm::inverse(light.transform.GetMatrix());

            shadowUniform.viewProjection = projection * view;

            lightsUniform.lights[count].color = {light.color, light.intensity};

            auto rotation = glm::quat(light.transform.basis.GetEulerNormalized());

            lightsUniform.lights[count].vector = glm::vec4{ rotation * glm::vec3{0.f, 0.f, 1.f}, 1.f};

            count++;

            break;
        }

        auto& pointLights = Renderer::Get().GetPointLights();

        for (auto& [_, light] : pointLights)
        {
            lightsUniform.lights[count].color = { light.color, light.range };
            lightsUniform.lights[count].vector = { light.transform.origin, 0.f };
            
            count++;
        }

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

        BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 4, 0);
    }

    void ForwardSubpass::BindShadowMap()
    {
        auto& shadowMap = shadowTarget->GetViews()[0];

        BindImage(*shadowMap, *shadowMapSampler, 0, 5, 0);
    }

    void ForwardSubpass::UpdateShadowUniform(Vulkan::CommandBuffer& commandBuffer, ShadowUniform uniform)
    {
        auto& frame = GetRenderContext().GetCurrentFrame();

        auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(ShadowUniform));

        allocation.SetData(&uniform);

        BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 6, 0);
    }
};