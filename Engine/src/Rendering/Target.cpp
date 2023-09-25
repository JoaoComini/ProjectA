#include "Target.hpp"

namespace Engine
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

	std::unique_ptr<Target> TargetBuilder::Build(const Vulkan::Device& device)
	{
		return std::make_unique<Target>(device, images);
	}
}