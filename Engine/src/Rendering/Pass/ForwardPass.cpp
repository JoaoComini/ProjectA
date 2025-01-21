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

        data.depth = builder.Allocate({
                1600,
                900,
                RenderTextureFormat::Depth,
                RenderTextureUsage::RenderTarget,
        });

        builder.Write(data.gbuffer, {
            .type = RenderTextureAccessType::Attachment,
            .attachment = {
                .aspect = RenderTextureAspect::Color,
            }
        });

        builder.Write(data.depth, {
            .type = RenderTextureAccessType::Attachment,
            .attachment = {
                .aspect = RenderTextureAspect::Depth,
            }
        });

        context.Add<ForwardPassData>(data);
    }

    struct CameraUniform
    {
        glm::mat4 viewProjectionMatrix;
        glm::vec3 position;
    };

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
        auto [camera, transform] = Renderer::Get().GetMainCamera();

        CameraUniform cameraUniform {
            .viewProjectionMatrix = camera.GetProjection() * glm::inverse(transform),
            .position = transform[3],
        };

        LightsUniform lights{};
        ShadowUniform shadow{};

        GetMainLightData(lights, shadow);
        GetAdditionalLightsData(lights);

        command.BindUniformBuffer(&cameraUniform, sizeof(CameraUniform), 0, 0);
        command.BindUniformBuffer(&lights, sizeof(LightsUniform), 0, 5);
        command.BindUniformBuffer(&shadow, sizeof(ShadowUniform), 0, 7);

        command.DrawGeometry(RenderGeometryType::Opaque, "forward");
        command.DrawGeometry(RenderGeometryType::Transparent, "forward");
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

        auto direction = glm::normalize(transform.rotation * glm::vec3{ 0, 0, 1 });
        auto view = glm::lookAt(-direction, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });

        Camera camera;
        camera.SetOrthographic(50, -25.f, 25.f);

        auto projection = camera.GetProjection();

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