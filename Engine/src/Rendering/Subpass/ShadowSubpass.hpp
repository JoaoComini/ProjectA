#pragma once

#include "Subpass.hpp"

#include "Scene/Scene.hpp"

namespace Engine
{
	class ShadowSubpass : public Subpass
	{
	public:
		ShadowSubpass(
			RenderContext& renderContext,
			Vulkan::ShaderSource&& vertexSource,
			Vulkan::ShaderSource&& fragmentSource,
			Scene& scene,
			Camera& shadowCamera
		);

		virtual void Draw(Vulkan::CommandBuffer& commandBuffer) override;

	protected:
		virtual Vulkan::PipelineLayout& GetPipelineLayout(const std::vector<Vulkan::ShaderModule*>& shaders) override;
		void UpdateGlobalUniform(Vulkan::CommandBuffer& commandBuffer, const glm::mat4& transform, const glm::mat4& viewProjection);
		std::pair<glm::mat4, glm::mat4> GetViewProjection() const;

	private:
		Scene& scene;
		Camera& shadowCamera;
	};
}