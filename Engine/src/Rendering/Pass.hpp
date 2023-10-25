#pragma once

#include "Subpass/Subpass.hpp"
#include "RenderTarget.hpp"

#include "Vulkan/RenderPass.hpp"

#include <vector>

namespace Engine
{

	class Pass
	{
	public:
		Pass(Vulkan::Device& device, std::vector<std::unique_ptr<Subpass>>&& subpasses);

		void Prepare(const RenderTarget& target);

		void Draw(Vulkan::CommandBuffer& commandBuffer);

		void SetLoadStoreInfos(std::vector<Vulkan::LoadStoreInfo> loadStoreInfos);

		Vulkan::RenderPass& GetRenderPass() const;
	private:
		std::unique_ptr<Vulkan::RenderPass> renderPass;
		std::vector<std::unique_ptr<Subpass>> subpasses;

		std::vector<Vulkan::LoadStoreInfo> loadStoreInfos{ 2 };
		std::vector<VkClearValue> clearValues{ 2 };

		VkExtent2D extent{0, 0};

		Vulkan::Device& device;
	};
}