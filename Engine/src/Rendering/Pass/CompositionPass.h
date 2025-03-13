#pragma once

#include "../RenderGraph/RenderGraphPass.h"

namespace Engine
{
	struct CompositionPassData { };

	class CompositionPass : public RenderGraphPass<CompositionPassData, RenderGraphCommand>
	{
	public:
		void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, CompositionPassData& data) override;
		void Render(RenderGraphCommand& command, const CompositionPassData& data) override;
	};
}