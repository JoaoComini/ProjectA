#pragma once

#include "GeometryPass.hpp"

#include "../RenderGraph/RenderGraphPass.hpp"

namespace Engine
{
	struct Light
	{
		glm::vec4 vector;
		glm::vec4 color;
	};

	struct alignas(16) LightsUniform
	{
		Light lights[32];
		int count;
	};

	struct ShadowUniform
	{
		glm::mat4 viewProjection;
	};

	struct ForwardPassData
	{
		RenderGraphResourceHandle gbuffer;
	};

	class ForwardPass : public GeometryPass, public RenderGraphPass<ForwardPassData>
	{
	public:
		ForwardPass(
			RenderContext& renderContext,
			ShaderSource&& vertexSource,
			ShaderSource&& fragmentSource,
			Scene& scene,
			RenderTarget* shadowTarget
		);

		void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ForwardPassData& data) override;
		void Render(RenderGraphCommand& command, const ForwardPassData& data) override;

		void Draw(Vulkan::CommandBuffer& commandBuffer) override;
	private:
		void CreateShadowMapSampler();

		void GetMainLightData(LightsUniform& lights, ShadowUniform& shadow);
		void GetAdditionalLightsData(LightsUniform& lights);

		void UpdateLightUniform(Vulkan::CommandBuffer& commandBuffer, LightsUniform uniform);

		void BindShadowMap(Vulkan::CommandBuffer& commandBuffer);

		void UpdateShadowUniform(Vulkan::CommandBuffer& commandBuffer, ShadowUniform uniform);
	private:
		RenderTarget* shadowTarget;
		std::unique_ptr<Vulkan::Sampler> shadowMapSampler;
	};
}