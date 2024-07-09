#pragma once

#include <vulkan/vulkan.h>

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/CommandBuffer.hpp"

#include <vector>

namespace Engine
{
	class RenderAttachment
	{
	public:
		RenderAttachment(Vulkan::Device& device, std::unique_ptr<Vulkan::Image>&& image, VkClearValue clearValue, Vulkan::LoadStoreInfo loadStore)
			: image(std::move(image)), clearValue(clearValue), loadStore(loadStore)
		{
			view = std::make_unique<Vulkan::ImageView>(device, *this->image);

			auto [width, height, _] = this->image->GetExtent();

			extent = { width, height };
		}

		VkFormat GetFormat() const
		{
			return image->GetFormat();
		}

		VkExtent2D GetExtent() const
		{
			return extent;
		}

		VkSampleCountFlagBits GetSampleCount() const
		{
			return image->GetSampleCount();
		}

		Vulkan::ImageView& GetView() const
		{
			return *view;
		}

		VkClearValue GetClearValue() const
		{
			return clearValue;
		}

		Vulkan::LoadStoreInfo GetLoadStoreInfo() const
		{
			return loadStore;
		}

		void SetResolve(std::unique_ptr<RenderAttachment>&& resolve)
		{
			this->resolve = std::move(resolve);
		}

		const std::unique_ptr<RenderAttachment>& GetResolve() const
		{
			return this->resolve;
		}

	private:
		std::unique_ptr<Vulkan::Image> image;
		std::unique_ptr<Vulkan::ImageView> view;

		VkClearValue clearValue;
		Vulkan::LoadStoreInfo loadStore;
		
		VkExtent2D extent;

		std::unique_ptr<RenderAttachment> resolve;
	};

	class RenderTarget
	{
	public:
		RenderTarget() = default;
		~RenderTarget() = default;

		RenderTarget& operator=(RenderTarget&& other) noexcept;

		VkExtent2D GetExtent() const;

		void AddColorAttachment(std::unique_ptr<RenderAttachment>&& attachment)
		{
			extent = attachment->GetExtent();

			colors.push_back(std::move(attachment));
		}

		void SetDepthAttachment(std::unique_ptr<RenderAttachment>&& attachment)
		{
			extent = attachment->GetExtent();

			depth = std::move(attachment);
		}

		const std::vector<std::unique_ptr<RenderAttachment>>& GetColorAttachments() const
		{
			return colors;
		}

		const std::unique_ptr<RenderAttachment>& GetDepthAttachment() const
		{
			return depth;
		}

	private:
		std::unique_ptr<RenderAttachment> depth;
		std::vector<std::unique_ptr<RenderAttachment>> colors;

		VkExtent2D extent{};
	};
}
