#pragma once

#include "Rendering/Subpass/Subpass.hpp"
#include "Rendering/RenderTarget.hpp"

#include <vector>

namespace Engine
{

	class Pass
	{
	public:
		Pass(Vulkan::Device& device, std::vector<std::unique_ptr<Subpass>>&& subpasses);

		void Draw(Vulkan::CommandBuffer& commandBuffer, RenderTarget& renderTarget);

		void SetLoadStoreInfos(std::vector<Vulkan::LoadStoreInfo> loadStoreInfos);

		void SetClearValues(std::vector<VkClearValue> clearValues);
	private:

		std::vector<std::unique_ptr<Subpass>> subpasses;

		std::vector<Vulkan::LoadStoreInfo> loadStoreInfos{ 2 };
		std::vector<VkClearValue> clearValues{ 2 };

		Vulkan::Device& device;
	};
}