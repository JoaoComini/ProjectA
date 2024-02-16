#pragma once

#include "GeometrySubpass.hpp"

#include <glm/glm.hpp>

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
			Camera& shadowCamera,
			RenderTarget* shadowTarget
		);

		void Draw(Vulkan::CommandBuffer& commandBuffer) override;
	private:
		void CreateShadowMapSampler();

		void UpdateLightUniform(Vulkan::CommandBuffer& commandBuffer, LightsUniform uniform);

		void BindShadowMap();

		void UpdateShadowUniform(Vulkan::CommandBuffer& commandBuffer, ShadowUniform uniform);
	private:
		Camera& shadowCamera;
		RenderTarget* shadowTarget;
		std::unique_ptr<Vulkan::Sampler> shadowMapSampler;

	};
}