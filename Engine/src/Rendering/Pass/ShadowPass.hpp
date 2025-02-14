#pragma once


#include "Scene/Scene.hpp"
#include "../RenderGraph/RenderGraphPass.hpp"
#include "../RenderGraph/RenderGraphResource.hpp"
#include "Rendering/RenderTexture.hpp"

namespace Engine
{
	struct ShadowSettings
	{
		float depthBias{ 4 };
		float normalBias{ 0.8 };
	};

	struct ShadowPassData
	{
		RenderGraphResourceHandle<RenderTexture> shadowmap;
		glm::vec3 lightDirection;
	};

	class ShadowPass : public RenderGraphPass<ShadowPassData, RenderGraphCommand>
	{
	public:
		ShadowPass(Scene& scene, ShadowSettings settings);

		void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ShadowPassData& data) override;
		void Render(RenderGraphCommand& builder, const ShadowPassData& command) override;
	private:
		Scene& scene;
		ShadowSettings settings;
	};
}
