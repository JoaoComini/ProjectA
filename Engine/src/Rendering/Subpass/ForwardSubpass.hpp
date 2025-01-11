#pragma once

#include "GeometrySubpass.hpp"

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

	class ForwardSubpass : public GeometrySubpass
	{
	public:
		ForwardSubpass(
			RenderContext& renderContext,
			Vulkan::ShaderSource&& vertexSource,
			Vulkan::ShaderSource&& fragmentSource,
			Scene& scene,
			RenderTarget* shadowTarget
		);

		void Draw(Vulkan::CommandBuffer& commandBuffer) override;
	private:
		void CreateShadowMapSampler();

		void GetMainLightData(LightsUniform& lights, ShadowUniform& shadow);
		void GetAdditionalLightsData(LightsUniform& lights);

		void UpdateLightUniform(Vulkan::CommandBuffer& commandBuffer, LightsUniform uniform);

		void BindShadowMap();

		void UpdateShadowUniform(Vulkan::CommandBuffer& commandBuffer, ShadowUniform uniform);
	private:
		RenderTarget* shadowTarget;
		std::unique_ptr<Vulkan::Sampler> shadowMapSampler;

	};
}