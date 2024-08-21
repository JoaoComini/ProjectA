#pragma once

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/CommandBuffer.hpp"

#include "RenderAttachment.hpp"

namespace Engine
{
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
