#pragma once

#include <vulkan/vulkan.h>

#include "Common/Hash.hpp"

#include "Device.hpp"
#include "Swapchain.hpp"
#include "Resource.hpp"

namespace Vulkan
{	
	struct AttachmentInfo
	{
		VkFormat format{ VK_FORMAT_UNDEFINED };
		VkSampleCountFlagBits samples{ VK_SAMPLE_COUNT_1_BIT };
		VkImageUsageFlags usage{ VK_IMAGE_USAGE_SAMPLED_BIT };
		VkImageLayout initialLayout{ VK_IMAGE_LAYOUT_UNDEFINED };
	};

	struct LoadStoreInfo
	{
		VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	};

	struct SubpassInfo
	{
		std::vector<uint32_t> inputAttachments;

		std::vector<uint32_t> outputAttachments;

		std::vector<uint32_t> colorResolveAttachments;

		bool disableDepthStencilAttachment;
	};

	class RenderPass : public Resource<VkRenderPass>
	{
	public:
		RenderPass(const Device& device, const std::vector<AttachmentInfo>& attachments, const std::vector<LoadStoreInfo>& loadStoreInfos, const std::vector<SubpassInfo>& subpasses);
		~RenderPass();

	private:
		const Device& device;
	};
};

namespace std
{
	template <>
	struct hash<Vulkan::RenderPass>
	{
		size_t operator()(const Vulkan::RenderPass& renderPass) const
		{
			std::size_t hash{ 0 };

			Hash(hash, renderPass.GetHandle());

			return hash;
		}
	};
};