#include "Framebuffer.hpp"

#include <algorithm>

#include "Rendering/RenderTarget.hpp"

#include "Device.hpp"

namespace Vulkan
{
	Framebuffer::Framebuffer(const Device& device, const RenderPass& renderPass, const Engine::RenderTarget& target): device(device)
	{
		std::vector<VkImageView> handles;
		
		for (auto& view : target.GetViews())
		{
			handles.emplace_back(view->GetHandle());
		}

		auto extent = target.GetExtent();

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass.GetHandle();
		framebufferInfo.attachmentCount = handles.size();
		framebufferInfo.pAttachments = handles.data();
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