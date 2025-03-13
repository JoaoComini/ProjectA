#pragma once

#include "Vulkan/Device.h"
#include "Vulkan/Image.h"
#include "Vulkan/ImageView.h"
#include "Vulkan/CommandBuffer.h"

#include "RenderAttachment.h"

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

		int GetColorAttachmentCount() const
		{
			return colors.size();
		}

		RenderAttachment& GetColorAttachment(int index) const
		{
			return *colors[index];
		}

		RenderAttachment* GetColorAttachmentPtr(int index) const
		{
			return colors[index].get();
		}
		
		bool HasDepthAttachment() const
		{
			return depth != nullptr;
		}

		const RenderAttachment& GetDepthAttachment() const
		{
			return *depth;
		}

	private:
		std::unique_ptr<RenderAttachment> depth;
		std::vector<std::unique_ptr<RenderAttachment>> colors;

		VkExtent2D extent{};
	};
}
