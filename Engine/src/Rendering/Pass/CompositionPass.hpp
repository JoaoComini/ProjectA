#pragma once

#include "Pass.hpp"
#include "../RenderGraph/RenderGraphPass.hpp"

namespace Engine
{
	struct CompositionPassData
	{

	};

	class CompositionPass : public Pass, public RenderGraphPass<CompositionPassData>
	{
	public:
		CompositionPass(
			RenderContext& renderContext,
			ShaderSource&& vertexSource,
			ShaderSource&& fragmentSource,
			RenderTarget* gBufferTarget
		);

		void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, CompositionPassData& data) override;
		void Render(RenderGraphCommand& command, const CompositionPassData& data) override;

		void Draw(Vulkan::CommandBuffer& commandBuffer) override;

	private:
		RenderTarget* gBufferTarget;

		std::unique_ptr<Vulkan::Sampler> gBufferSampler;
	};
}