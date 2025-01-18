#pragma once

#include "Pass.hpp"

#include "Scene/Scene.hpp"
#include "../RenderGraph/RenderGraphPass.hpp"

namespace Engine
{
	struct ShadowPassData
	{
		RenderGraphResourceHandle shadowmap;
	};

	class ShadowPass : public Pass, public RenderGraphPass<ShadowPassData>
	{
	public:
		ShadowPass(
			RenderContext& renderContext,
			ShaderSource&& vertexSource,
			ShaderSource&& fragmentSource,
			Scene& scene
		);

		void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ShadowPassData& data) override;
		void Render(RenderGraphCommand& builder, const ShadowPassData& command) override;

		virtual void Draw(Vulkan::CommandBuffer& commandBuffer) override;

	protected:
		virtual Vulkan::PipelineLayout& GetPipelineLayout(const std::vector<Shader*>& shaders) override;
		void UpdateGlobalUniform(Vulkan::CommandBuffer& commandBuffer, const glm::mat4& transform, const glm::mat4& viewProjection);
		std::pair<glm::mat4, glm::mat4> GetViewProjection() const;

	private:
		Scene& scene;
	};
}