#include "Framebuffer.hpp"

#include <algorithm>

namespace Vulkan
{
	Framebuffer::Framebuffer(const Device& device, const RenderPass& renderPass, std::vector<std::unique_ptr<ImageView>>& attachments, VkExtent2D extent): device(device)
	{
		std::vector<VkImageView> views;
		
		for (auto& attachment : attachments)
		{
			views.emplace_back(attachment->GetHandle());
		}

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass.GetHandle();
		framebufferInfo.attachmentCount = views.size();
		framebufferInfo.pAttachments = views.data();
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device.GetHandle(), &framebufferInfo, nullptr, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

	Framebuffer::~Framebuffer()
	{
		vkDestroyFramebuffer(device.GetHandle(), handle, nullptr);
	}
}