#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"

namespace Rendering
{
	class Target
	{
	public:
		Target(const Vulkan::Device& device, std::vector<std::unique_ptr<Vulkan::Image>>& images);
		~Target() = default;

		std::vector<std::unique_ptr<Vulkan::ImageView>>& GetViews();

	private:
		std::vector<std::unique_ptr<Vulkan::Image>> images;
		std::vector<std::unique_ptr<Vulkan::ImageView>> views;

	};

	class TargetBuilder
	{
	public:
		TargetBuilder() = default;

		TargetBuilder& AddImage(std::unique_ptr<Vulkan::Image> image);

		std::unique_ptr<Target> Build(const Vulkan::Device& device);

	private:
		VkExtent2D extent;
		bool enableDepthStencil = false;

		std::vector<std::unique_ptr<Vulkan::Image>> images;
	};
}