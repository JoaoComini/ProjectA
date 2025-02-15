#pragma once

#include "Rendering/RenderGraph/RenderGraphResource.hpp"
#include "Rendering/RenderGraph/RenderGraphPass.hpp"
#include "Rendering/RenderTexture.hpp"

namespace Engine
{
	class Scene;
	struct LightsUniform;
	struct ShadowUniform;

	struct ForwardPassData
	{
		RenderGraphResourceHandle<RenderTexture> gBuffer;
		RenderGraphResourceHandle<RenderTexture> depth;
	};

	class ForwardPass : public RenderGraphPass<ForwardPassData, RenderGraphCommand>
	{
	public:
		explicit ForwardPass(Scene& scene);

		void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ForwardPassData& data) override;
		void Render(RenderGraphCommand& command, const ForwardPassData& data) override;
	private:
		Scene& scene;
	};
}
