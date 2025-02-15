#include "CompositionPass.hpp"

#include "Rendering/Renderer.hpp"

#include "Rendering/RenderGraphCommand.hpp"

namespace Engine
{
    void CompositionPass::RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, CompositionPassData& data)
    {
        const auto& gBuffer = context.Get<ForwardPassData>().gBuffer;

        builder.Read(gBuffer, {
            .type = RenderTextureAccessType::Binding,
            .binding = {
                .set = 0,
                .location = 0,
            }
        });

        const auto& backBuffer = context.Get<BackBufferData>().target;

        builder.Write(backBuffer, {
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