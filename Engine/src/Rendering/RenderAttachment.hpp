
#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/CommandBuffer.hpp"

namespace Engine
{
	struct BarrierScope
	{
		VkAccessFlags access{ 0 };
		VkPipelineStageFlags stage{ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };
	};

	class RenderAttachment
	{
	public:
		RenderAttachment(
			Vulkan::Device& device,
			std::unique_ptr<Vulkan::Image>&& image,
			VkClearValue clearValue,
			Vulkan::LoadStoreInfo loadStore
		);

		VkFormat GetFormat() const;
		VkExtent2D GetExtent() const;
		VkSampleCountFlagBits GetSampleCount() const;
		Vulkan::ImageView& GetView() const;
		VkClearValue GetClearValue() const;

		Vulkan::LoadStoreInfo GetLoadStoreInfo() const;

		BarrierScope& GetScope();
		VkImageLayout& GetLayout();

		void SetResolve(std::unique_ptr<RenderAttachment>&& resolve);
		const std::unique_ptr<RenderAttachment>& GetResolve() const;

	private:
		std::unique_ptr<Vulkan::Image> image;
		std::unique_ptr<Vulkan::ImageView> view;

		VkClearValue clearValue;
		Vulkan::LoadStoreInfo loadStore;

		VkExtent2D extent;
		BarrierScope scope;
		VkImageLayout layout{ VK_IMAGE_LAYOUT_UNDEFINED };

		std::unique_ptr<RenderAttachment> resolve;

	public:
		class Builder
		{

		public:
			Builder(Vulkan::Device& device);

			Builder& Usage(VkImageUsageFlags usage);
			Builder& Format(VkFormat format);
			Builder& Extent(VkExtent2D extent);
			Builder& SampleCount(VkSampleCountFlagBits sampleCount);
			Builder& ClearValue(VkClearValue clearValue);
			Builder& LoadStoreInfo(Vulkan::LoadStoreInfo loadStoreInfo);
			Builder& Resolve(std::unique_ptr<RenderAttachment>&& resolve);
			std::unique_ptr<RenderAttachment> Build();

		private:
			Vulkan::Device& device;

			VkImageUsageFlags usage{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
			VkFormat format{ VK_FORMAT_UNDEFINED };
			VkExtent2D extent{};
			VkSampleCountFlagBits sampleCount{ VK_SAMPLE_COUNT_1_BIT };
			VkClearValue clearValue{};
			Vulkan::LoadStoreInfo loadStoreInfo{};

			std::unique_ptr<RenderAttachment> resolve{ nullptr };
		};
	};

}