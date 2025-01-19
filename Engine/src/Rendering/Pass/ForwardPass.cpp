#include "ForwardPass.hpp"

#include "Rendering/Renderer.hpp"

#include "Rendering/RenderGraph/RenderGraphCommand.hpp"

namespace Engine
{
    ForwardPass::ForwardPass(Scene& scene) : scene(scene) { }

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

        data.gbuffer = builder.Allocate({
                1600,
                900,
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

    struct Light
    {
        glm::vec4 vector;
        glm::vec4 color;
    };

    struct alignas(16) LightsUniform
    {
        Light lights[32];
        int count;
    };

    struct ShadowUniform
    {
        glm::mat4 viewProjection;
    };

    void ForwardPass::Render(RenderGraphCommand& command, const ForwardPassData& data)
    {
        LightsUniform lights{};
        ShadowUniform shadow{};

        GetMainLightData(lights, shadow);
        GetAdditionalLightsData(lights);

        command.BindUniformBuffer(&lights, sizeof(LightsUniform), 0, 5);
        command.BindUniformBuffer(&lights, sizeof(ShadowUniform), 0, 7);

        command.DrawGeometry(RenderGeometryType::Opaque, "forward");
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
};