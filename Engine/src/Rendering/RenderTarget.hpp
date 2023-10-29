#pragma once

#include <vulkan/vulkan.h>

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/RenderPass.hpp"
#include "Vulkan/Framebuffer.hpp"

#include <vector>

namespace Engine
{
	class RenderTarget
	{
	public:
		RenderTarget(Vulkan::Device& device, std::vector<std::unique_ptr<Vulkan::Image>>&& images);
		~RenderTarget() = default;

		const std::vector<std::unique_ptr<Vulkan::ImageView>>& GetViews() const;
		const std::vector<Vulkan::AttachmentInfo>& GetAttachments() const;

		VkExtent2D GetExtent() const;

	private:
		std::vector<std::unique_ptr<Vulkan::Image>> images;
		std::vector<std::unique_ptr<Vulkan::ImageView>> views;

		std::vector<Vulkan::AttachmentInfo> attachments;

		VkExtent2D extent{};

		Vulkan::Device& device;
	};
}

namespace std
{
	template <>
	struct hash<Engine::RenderTarget>
	{
		size_t operator()(const Engine::RenderTarget& target) const
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
