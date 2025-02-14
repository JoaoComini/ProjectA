#pragma once

#include <Rendering/RenderGraph/RenderGraphResource.hpp>
#include "../RenderGraph/RenderGraphPass.hpp"
#include "Rendering/RenderTexture.hpp"

namespace Engine
{
	class Scene;
	struct LightsUniform;
	struct ShadowUniform;

	struct ForwardPassData
	{
		RenderGraphResourceHandle<RenderTexture> gbuffer;
		RenderGraphResourceHandle<RenderTexture> depth;
	};

	class ForwardPass : public RenderGraphPass<ForwardPassData, RenderGraphCommand>
	{
	public:
		ForwardPass(Scene& scene);

		void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ForwardPassData& data) override;
		void Render(RenderGraphCommand& command, const ForwardPassData& data) override;
	private:
		void GetMainLightData(LightsUniform& lights, ShadowUniform& shadow);
		void GetAdditionalLightsData(LightsUniform& lights);
	private:
		Scene& scene;
	};
}
