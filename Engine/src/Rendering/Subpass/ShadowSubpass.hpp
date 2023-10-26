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
		virtual void PreparePipelineLayout() override;
		virtual void PreparePipeline(Vulkan::RenderPass& renderPass) override;

		virtual glm::mat4 GetViewProjection() const override;

	private:
		Camera& shadowCamera;
	};
}