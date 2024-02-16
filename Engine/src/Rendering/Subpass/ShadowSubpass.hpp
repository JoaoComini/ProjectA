#pragma once

#include "GeometrySubpass.hpp"

#include "Scene/Scene.hpp"

namespace Engine
{
	class ShadowSubpass : public GeometrySubpass
	{
	public:
		ShadowSubpass(
			RenderContext& renderContext,
			Vulkan::ShaderSource&& vertexSource,
			Vulkan::ShaderSource&& fragmentSource,
			Scene& scene,
			Camera& shadowCamera
		);

	protected:
		virtual Vulkan::PipelineLayout& GetPipelineLayout(const std::vector<Vulkan::ShaderModule*>& shaders) override;
		virtual void PreparePipelineState(Vulkan::CommandBuffer& commandBuffer) override;

		virtual std::pair<glm::mat4, glm::mat4> GetViewProjection() const override;

	private:
		Camera& shadowCamera;
	};
}