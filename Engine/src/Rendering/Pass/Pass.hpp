#pragma once

#include "Rendering/Subpass/Subpass.hpp"
#include "Rendering/RenderTarget.hpp"

#include "Vulkan/RenderPass.hpp"

#include <vector>

namespace Engine
{

	class Pass
	{
	public:
		Pass(Vulkan::Device& device, std::vector<std::unique_ptr<Subpass>>&& subpasses);

		void Prepare(const RenderTarget& target);

		void Draw(Vulkan::CommandBuffer& commandBuffer, RenderTarget& renderTarget);

		void SetLoadStoreInfos(std::vector<Vulkan::LoadStoreInfo> loadStoreInfos);

		void SetClearValues(std::vector<VkClearValue> clearValues);

		Vulkan::RenderPass& GetRenderPass() const;
	private:
		std::unique_ptr<Vulkan::RenderPass> renderPass;
		std::vector<std::unique_ptr<Subpass>> subpasses;

		std::vector<Vulkan::LoadStoreInfo> loadStoreInfos{ 2 };
		std::vector<VkClearValue> clearValues{ 2 };

		Vulkan::Device& device;
	};
}