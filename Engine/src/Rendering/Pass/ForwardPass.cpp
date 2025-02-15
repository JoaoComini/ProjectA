#include "ForwardPass.hpp"

#include "Rendering/Renderer.hpp"

#include "Rendering/RenderGraphCommand.hpp"

namespace Engine
{
    ForwardPass::ForwardPass(Scene& scene) : scene(scene) { }

    void ForwardPass::RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ForwardPassData& data)
    {
        const auto& frame = context.Get<FrameData>();

        builder.Read(frame.camera, {
            .set = 0,
            .binding = 0,
        });

        const auto& lights = context.Get<LightData>();

        builder.Read(lights.lights, {
            .set = 0,
            .binding = 5,
        });

        builder.Read(lights.shadows, {
            .set = 0,
            .binding = 7,
        });

        const auto& shadow = context.Get<ShadowPassData>();

        builder.Read(shadow.shadowMap, {
            .type = RenderTextureAccessType::Binding,
            .binding = {
                .set = 0,
                .location = 6,
                .sampler = RenderTextureSampler::Shadow
            }
        });

        data.gBuffer = builder.Allocate<RenderTexture>({
                1600,
                900,
                RenderTextureFormat::HDR,
                RenderTextureUsage::RenderTarget | RenderTextureUsage::Sampled,
        });

        data.depth = builder.Allocate<RenderTexture>({
                1600,
                900,
                RenderTextureFormat::Depth,
                RenderTextureUsage::RenderTarget,
        });

        builder.Write(data.gBuffer, {
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

    void ForwardPass::Render(RenderGraphCommand& command, const ForwardPassData& data)
    {
        command.DrawGeometry({ RenderGeometryType::Opaque, "forward" });
        command.DrawGeometry({ RenderGeometryType::Transparent, "forward" });
    }
};