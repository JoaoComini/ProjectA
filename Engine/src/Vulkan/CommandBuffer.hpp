#pragma once

#include <vulkan/vulkan.h>

#include <vector>
 
#include "Resource.hpp"

#include "PipelineState.hpp"

namespace Vulkan
{
	class CommandPool;
	class Device;
	class Framebuffer;
	class Pipeline;
	class PipelineLayout;
	class Image;
	class ImageView;
	class Buffer;

	struct AttachmentInfo
	{
		VkFormat format{ VK_FORMAT_UNDEFINED };
		VkSampleCountFlagBits samples{ VK_SAMPLE_COUNT_1_BIT };
		VkImageUsageFlags usage{ VK_IMAGE_USAGE_SAMPLED_BIT };
		VkImageLayout initialLayout{ VK_IMAGE_LAYOUT_UNDEFINED };
	};

	struct LoadStoreInfo
	{
		VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	};

	struct ImageMemoryBarrierInfo
	{
		VkPipelineStageFlags srcStageMask{ VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
		VkPipelineStageFlags dstStageMask{ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };

		VkAccessFlags srcAccessMask{ 0 };
		VkAccessFlags dstAccessMask{ 0 };

		VkImageLayout oldLayout{ VK_IMAGE_LAYOUT_UNDEFINED };
		VkImageLayout newLayout{ VK_IMAGE_LAYOUT_UNDEFINED };

		uint32_t oldQueueFamily{ VK_QUEUE_FAMILY_IGNORED };
		uint32_t newQueueFamily{ VK_QUEUE_FAMILY_IGNORED };
	};

	class CommandBuffer : public Resource<VkCommandBuffer>
	{
	public:
		enum class Level
		{
			Primary,
			Secondary
		};

		enum class BeginFlags
		{
			None = 0x0,
			OneTimeSubmit = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		};

		CommandBuffer(const Device& device, const CommandPool& commandPool, const Level level = Level::Primary);
		~CommandBuffer();

		void Begin(BeginFlags flags = BeginFlags::None);
		void End();

		void BeginRendering(const std::vector<AttachmentInfo> attachments, const std::vector<std::unique_ptr<ImageView>>& views, const std::vector<VkClearValue>& clearValues, const std::vector<LoadStoreInfo> loadStoreInfos, VkExtent2D extent);
		void EndRendering();

		void SetViewport(const std::vector<VkViewport>& viewports);
		void SetScissor(const std::vector<VkRect2D>& scissors);
		void SetVertexInputState(const VertexInputState& state);
		void SetMultisampleState(const MultisampleState& state);
		void SetInputAssemblyState(const InputAssemblyState& state);
		void SetRasterizationState(const RasterizationState& state);
		void SetDepthStencilState(const DepthStencilState& state);

		void BindPipelineLayout(PipelineLayout& pipelineLayout);
		void BindDescriptorSet(VkDescriptorSet descriptorSet);
		void PushConstants(VkShaderStageFlags stages, uint32_t offset, uint32_t size, void* data);

		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);

		void Free();

		void CopyBuffer(VkBuffer src, VkBuffer dst, uint32_t size);
		void CopyBufferToImage(const Buffer& buffer, const Image& image, const std::vector<VkBufferImageCopy>& regions);

		void SetImageLayout(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageSubresourceRange subresourceRange);
		void GenerateMipMaps(const Image& image);

		void ImageMemoryBarrier(const ImageView& imageView, const ImageMemoryBarrierInfo& barrier);

	private:
		void Flush();

		const Device& device;
		const CommandPool& commandPool;

		PipelineState pipelineState{};
	};
}
