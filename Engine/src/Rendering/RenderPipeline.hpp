#pragma once

#include "Pass.hpp"

#include "Scene/Scene.hpp"

namespace Engine
{
	class RenderPipeline
	{
	public:
		RenderPipeline(Vulkan::Device& device, Scene& scene);

		void Draw(Vulkan::CommandBuffer& commandBuffer);

		Vulkan::RenderPass& GetMainRenderPass() const;
	private:
		void SetupMainPass();
		void SetupShadowPass();

		std::unique_ptr<RenderTarget> CreateShadowPassTarget();

		void DrawShadowPass(Vulkan::CommandBuffer& commandBuffer);
		void DrawMainPass(Vulkan::CommandBuffer& commandBuffer);

		void SetViewportAndScissor(Vulkan::CommandBuffer& commandBuffer, VkExtent2D extent);

		std::unique_ptr<Pass> mainPass;
		std::unique_ptr<Pass> shadowPass;

		std::unique_ptr<Camera> shadowCamera;

		std::vector<std::unique_ptr<RenderTarget>> shadowPassTargets;

		Scene& scene;
		Vulkan::Device& device;
	};
}