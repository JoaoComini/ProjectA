#pragma once

#include "GeometrySubpass.hpp"

#include "Scene/Scene.hpp"

namespace Engine
{
	class ShadowSubpass : public GeometrySubpass
	{
	public:
		ShadowSubpass(
			Vulkan::Device& device,
			Vulkan::ShaderSource&& vertexSource,
			Vulkan::ShaderSource&& fragmentSource,
			Scene& scene,
			Camera& shadowCamera
		);

	protected:
		virtual Vulkan::PipelineLayout& GetPipelineLayout(const std::vector<Vulkan::ShaderModule*>& shaders) override;
		virtual Vulkan::Pipeline& GetPipeline(Vulkan::PipelineLayout& pipelineLayout, Vulkan::PipelineSpec& spec) override;

		virtual std::pair<glm::mat4, glm::mat4> GetViewProjection() const override;

	private:
		Camera& shadowCamera;
	};
}