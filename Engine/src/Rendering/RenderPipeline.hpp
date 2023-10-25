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
		std::unique_ptr<Pass> mainPass;
	};
}