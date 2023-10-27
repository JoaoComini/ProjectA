#pragma once

#include "GeometrySubpass.hpp"

#include <glm/glm.hpp>

namespace Engine
{
	struct LightUniform
	{
		glm::vec4 vector;
		glm::vec4 color;
	};

	struct ShadowUniform
	{
		glm::mat4 viewProjection;
	};

	class ForwardSubpass : public GeometrySubpass
	{
	public:
		ForwardSubpass(
			Vulkan::Device& device,
			Vulkan::ShaderSource&& vertexSource,
			Vulkan::ShaderSource&& fragmentSource,
			Scene& scene,
			Camera& shadowCamera,
			std::vector<std::unique_ptr<RenderTarget>>& shadowRenderTargets
		);


		void Draw(Vulkan::CommandBuffer& commandBuffer) override;
	private:
		void CreateShadowMapSampler();

		void UpdateLightUniform(Vulkan::CommandBuffer& commandBuffer, LightUniform uniform);

		void BindShadowMap();

		void UpdateShadowUniform(Vulkan::CommandBuffer& commandBuffer, ShadowUniform uniform);

	private:
		Camera& shadowCamera;
		std::vector<std::unique_ptr<RenderTarget>>& shadowRenderTargets;
		std::unique_ptr<Vulkan::Sampler> shadowMapSampler;

	};
}