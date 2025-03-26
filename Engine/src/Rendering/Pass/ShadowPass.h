#pragma once


#include "Scene/SceneGraph.h"
#include "Rendering/RenderGraph/RenderGraphPass.h"
#include "Rendering//RenderGraph/RenderGraphResource.h"
#include "Rendering/RenderTexture.h"

namespace Engine
{
	struct ShadowSettings
	{
		float depthBias{ 4 };
		float normalBias{ 0.8 };
	};

	struct ShadowPassData
	{
		RenderGraphResourceHandle<RenderTexture> shadowMap;
		glm::vec3 lightDirection;
	};

	class ShadowPass final : public RenderGraphPass<ShadowPassData, RenderGraphCommand>
	{
	public:
		ShadowPass(SceneGraph& scene, ShadowSettings settings);

		void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ShadowPassData& data) override;
		void Render(RenderGraphCommand& command, const ShadowPassData& data) override;
	private:
		SceneGraph& scene;
		ShadowSettings settings;
	};
}
