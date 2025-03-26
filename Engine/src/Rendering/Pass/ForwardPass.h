#pragma once

#include "Rendering/Renderer.h"

#include "Rendering/RenderGraph/RenderGraphResource.h"
#include "Rendering/RenderGraph/RenderGraphPass.h"
#include "Rendering/RenderTexture.h"

namespace Engine
{
	class SceneGraph;

	struct ResolutionSettings
	{
		uint32_t width;
		uint32_t height;
	};

	struct ForwardPassData
	{
		RenderGraphResourceHandle<RenderTexture> gBuffer;
		RenderGraphResourceHandle<RenderTexture> depth;
	};

	class ForwardPass : public RenderGraphPass<ForwardPassData, RenderGraphCommand>
	{
	public:
		explicit ForwardPass(SceneGraph& scene, ResolutionSettings settings);

		void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ForwardPassData& data) override;
		void Render(RenderGraphCommand& command, const ForwardPassData& data) override;
	private:
		SceneGraph& scene;
		ResolutionSettings settings;
	};
}
