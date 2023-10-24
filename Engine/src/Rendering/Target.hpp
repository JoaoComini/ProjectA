#pragma once

#include <vulkan/vulkan.h>

#include "Common/Hash.hpp"

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"

#include <vector>

namespace Engine
{
	class Target
	{
	public:
		Target(const Vulkan::Device& device, std::vector<std::unique_ptr<Vulkan::Image>>&& images);
		~Target() = default;

		const std::vector<std::unique_ptr<Vulkan::ImageView>>& GetViews() const;

		VkExtent2D GetExtent() const;

	private:
		std::vector<std::unique_ptr<Vulkan::Image>> images;
		std::vector<std::unique_ptr<Vulkan::ImageView>> views;

		VkExtent2D extent{};
	};
}

namespace std
{
	template <>
	struct hash<Engine::Target>
	{
		size_t operator()(const Engine::Target& target) const
		{
			std::size_t result{ 0 };

			for (auto& view : target.GetViews())
			{
				HashCombine(result, view->GetHandle());
			}

			return result;
		}
	};
};