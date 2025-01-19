#pragma once

#include "Scene/Scene.hpp"
#include "../RenderGraph/RenderGraphPass.hpp"

namespace Engine
{
	struct ShadowPassData
	{
		RenderGraphResourceHandle shadowmap;
		glm::vec3 lightDirection;
	};

	class ShadowPass : public RenderGraphPass<ShadowPassData>
	{
	public:
		ShadowPass(Scene& scene);

		void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ShadowPassData& data) override;
		void Render(RenderGraphCommand& builder, const ShadowPassData& command) override;
	private:
		Scene& scene;
	};
}