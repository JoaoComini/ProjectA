#pragma once

#include "../RenderGraph/RenderGraphPass.hpp"
#include "Vulkan/CommandBuffer.hpp"

namespace Engine
{
	class Scene;
	struct LightsUniform;
	struct ShadowUniform;

	struct ForwardPassData
	{
		RenderGraphResourceHandle gbuffer;
		RenderGraphResourceHandle depth;
	};

	class ForwardPass : public RenderGraphPass<ForwardPassData>
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