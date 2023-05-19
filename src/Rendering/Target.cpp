#include "Target.hpp"

namespace Rendering
{

	Target::Target(const Vulkan::Device& device, std::vector<std::unique_ptr<Vulkan::Image>>& images) : images(std::move(images))
	{
		for (auto& image : this->images)
		{
			views.push_back(std::make_unique<Vulkan::ImageView>(device, *image));
		}
	}

	std::vector<std::unique_ptr<Vulkan::ImageView>>& Target::GetViews()
	{
		return views;
	}


	TargetBuilder& TargetBuilder::AddImage(std::unique_ptr<Vulkan::Image> image)
	{
		images.push_back(std::move(image));
		return *this;
	}


	TargetBuilder& TargetBuilder::EnableDepthStencil()
	{
		enableDepthStencil = true;
		return *this;
	}

	TargetBuilder& TargetBuilder::Extent(VkExtent2D extent)
	{
		this->extent = extent;
		return *this;
	}

	std::unique_ptr<Target> TargetBuilder::Build(const Vulkan::Device& device)
	{

		if (enableDepthStencil)
		{
			images.push_back(std::make_unique<Vulkan::Image>(device, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D32_SFLOAT, extent.width, extent.height));
		}

		return std::make_unique<Target>(device, images);
	}
}