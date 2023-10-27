#include "RenderTarget.hpp"

#include <set>
#include <iterator>

#include "Caching/FramebufferCache.hpp"

namespace Engine
{

	struct CompareExtent2D
	{
		bool operator()(const VkExtent2D& lhs, const VkExtent2D& rhs) const
		{
			return !(lhs.width == rhs.width && lhs.height == rhs.height) && (lhs.width < rhs.width && lhs.height < rhs.height);
		}
	};

	RenderTarget::RenderTarget(const Vulkan::Device& device, std::vector<std::unique_ptr<Vulkan::Image>>&& images) : images(std::move(images))
	{
		std::set<VkExtent2D, CompareExtent2D> unique;
		std::transform(this->images.begin(), this->images.end(), std::inserter(unique, unique.end()), [](const std::unique_ptr<Vulkan::Image>& image) {
			return VkExtent2D{ image->GetExtent().width, image->GetExtent().height }; }
		);

		// Check if all render target images have the same size
		if (unique.size() != 1)
		{
			throw std::runtime_error("render target images must have the same extent");
		}

		extent = *unique.begin();

		for (auto& image : this->images)
		{
			views.push_back(std::make_unique<Vulkan::ImageView>(device, *image));

			attachments.push_back({image->GetFormat(), image->GetSampleCount(), image->GetUsage()});
		}

		framebufferCache = std::make_unique<FramebufferCache>(device);
	}

	const std::vector<std::unique_ptr<Vulkan::ImageView>>& RenderTarget::GetViews() const
	{
		return views;
	}

	const std::vector<Vulkan::AttachmentInfo>& RenderTarget::GetAttachments() const
	{
		return attachments;
	}

	Vulkan::Framebuffer& RenderTarget::RequestFramebuffer(const Vulkan::RenderPass& renderPass)
	{
		return framebufferCache->RequestFramebuffer(renderPass, *this);
	}

	VkExtent2D RenderTarget::GetExtent() const
	{
		return extent;
	}
}