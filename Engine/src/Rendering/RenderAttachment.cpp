#include "RenderAttachment.hpp"

namespace Engine
{
	RenderAttachment::RenderAttachment(Vulkan::Device& device, std::unique_ptr<Vulkan::Image>&& image, VkClearValue clearValue, Vulkan::LoadStoreInfo loadStore)
		: image(std::move(image)), clearValue(clearValue), loadStore(loadStore)
	{
		view = std::make_unique<Vulkan::ImageView>(device, *this->image);

		auto [width, height, _] = this->image->GetExtent();

		extent = { width, height };
	}

	VkFormat RenderAttachment::GetFormat() const
	{
		return image->GetFormat();
	}

	VkExtent2D RenderAttachment::GetExtent() const
	{
		return extent;
	}

	VkSampleCountFlagBits RenderAttachment::GetSampleCount() const
	{
		return image->GetSampleCount();
	}

	Vulkan::ImageView& RenderAttachment::GetView() const
	{
		return *view;
	}

	VkClearValue RenderAttachment::GetClearValue() const
	{
		return clearValue;
	}

	Vulkan::LoadStoreInfo RenderAttachment::GetLoadStoreInfo() const
	{
		return loadStore;
	}

	void RenderAttachment::SetResolve(std::unique_ptr<RenderAttachment>&& resolve)
	{
		this->resolve = std::move(resolve);
	}

	const std::unique_ptr<RenderAttachment>& RenderAttachment::GetResolve() const
	{
		return this->resolve;
	}

	RenderAttachment::Builder::Builder(Vulkan::Device& device)
		: device(device)
	{
	}

	RenderAttachment::Builder& RenderAttachment::Builder::Usage(VkImageUsageFlags usage)
	{
		this->usage = usage;

		return *this;
	}

	RenderAttachment::Builder& RenderAttachment::Builder::Format(VkFormat format)
	{
		this->format = format;

		return *this;
	}

	RenderAttachment::Builder& RenderAttachment::Builder::Extent(VkExtent2D extent)
	{
		this->extent = extent;

		return *this;
	}

	RenderAttachment::Builder& RenderAttachment::Builder::SampleCount(VkSampleCountFlagBits sampleCount)
	{
		this->sampleCount = sampleCount;

		return *this;
	}

	RenderAttachment::Builder& RenderAttachment::Builder::ClearValue(VkClearValue clearValue)
	{
		this->clearValue = clearValue;

		return *this;
	}

	RenderAttachment::Builder& RenderAttachment::Builder::LoadStoreInfo(Vulkan::LoadStoreInfo loadStoreInfo)
	{
		this->loadStoreInfo = loadStoreInfo;

		return *this;
	}

	RenderAttachment::Builder& RenderAttachment::Builder::Resolve(std::unique_ptr<RenderAttachment>&& resolve)
	{
		this->resolve = std::move(resolve);

		return *this;
	}

	std::unique_ptr<RenderAttachment> RenderAttachment::Builder::Build()
	{
		auto image = std::make_unique<Vulkan::Image>(
			device,
			usage,
			format,
			VkExtent3D{ extent.width, extent.height, 1 },
			sampleCount
		);

		auto attachment = std::make_unique<RenderAttachment>(
			device,
			std::move(image),
			clearValue,
			loadStoreInfo
		);

		if (resolve)
		{
			attachment->SetResolve(std::move(resolve));
		}

		return std::move(attachment);
	}
}
