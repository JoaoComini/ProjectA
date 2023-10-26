#pragma once

#include <vulkan/vulkan.h>

#include "Common/Hash.hpp"

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/RenderPass.hpp"

#include "Caching/FramebufferCache.hpp"

#include <vector>

namespace Engine
{
	class RenderTarget
	{
	public:
		RenderTarget(const Vulkan::Device& device, std::vector<std::unique_ptr<Vulkan::Image>>&& images);
		~RenderTarget() = default;

		const std::vector<std::unique_ptr<Vulkan::ImageView>>& GetViews() const;
		const std::vector<Vulkan::AttachmentInfo>& GetAttachments() const;

		Vulkan::Framebuffer& RequestFramebuffer(const Vulkan::RenderPass& renderPass);

		VkExtent2D GetExtent() const;

	private:
		std::vector<std::unique_ptr<Vulkan::Image>> images;
		std::vector<std::unique_ptr<Vulkan::ImageView>> views;

		std::vector<Vulkan::AttachmentInfo> attachments;

		VkExtent2D extent{};

		std::unique_ptr<FramebufferCache> framebufferCache;
	};
}
