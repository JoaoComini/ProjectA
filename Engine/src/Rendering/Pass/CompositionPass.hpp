#pragma once

#include "../RenderGraph/RenderGraphPass.hpp"

namespace Engine
{
	struct CompositionPassData { };

	class CompositionPass : public RenderGraphPass<CompositionPassData>
	{
	public:
		void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, CompositionPassData& data) override;
		void Render(RenderGraphCommand& command, const CompositionPassData& data) override;
	};
}