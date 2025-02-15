#include "CompositionPass.hpp"

#include "Rendering/Renderer.hpp"

#include "Rendering/RenderGraphCommand.hpp"

namespace Engine
{
    void CompositionPass::RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, CompositionPassData& data)
    {
        auto& gbuffer = context.Get<ForwardPassData>().gBuffer;

        builder.Read(gbuffer, {
            .type = RenderTextureAccessType::Binding,
            .binding = {
                .set = 0,
                .location = 0,
            }
        });

        auto backbuffer = context.Get<BackbufferData>().target;

        builder.Write(backbuffer, {
            .type = RenderTextureAccessType::Attachment,
            .attachment = {
                .aspect = RenderTextureAspect::Color,
            },
        });
    }

    void CompositionPass::Render(RenderGraphCommand& command, const CompositionPassData& data)
    {
        command.Blit("composition");
    }
}