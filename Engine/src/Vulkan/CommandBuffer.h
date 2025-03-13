#pragma once


#include "Resource.h"

#include "PipelineState.h"

#include <variant>

namespace Vulkan
{
	template <typename T>
	using BindingMap = std::map<uint32_t, std::map<uint32_t, T>>;

	class CommandPool;
	class Device;
	class Framebuffer;
	class Pipeline;
	class PipelineLayout;
	class Image;
	class Sampler;
	class ImageView;
	class Buffer;

	struct LoadStoreInfo
	{
		VkAttachmentLoadOp loadOp{ VK_ATTACHMENT_LOAD_OP_CLEAR };
		VkAttachmentStoreOp storeOp{ VK_ATTACHMENT_STORE_OP_STORE };
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

		CommandBuffer(Device& device, CommandPool& commandPool, Level level = Level::Primary);
		~CommandBuffer();

		void Begin(BeginFlags flags = BeginFlags::None);
		void End();

		void BeginRendering(const VkRenderingInfo& renderingInfo);
		void EndRendering();

		void SetViewport(const std::vector<VkViewport>& viewports);
		void SetScissor(const std::vector<VkRect2D>& scissors);
		void SetVertexInputState(const VertexInputState& state);
		void SetMultisampleState(const MultisampleState& state);
		void SetInputAssemblyState(const InputAssemblyState& state);
		void SetRasterizationState(const RasterizationState& state);
		void SetDepthStencilState(const DepthStencilState& state);
		void SetPipelineRenderingState(const PipelineRenderingState& state);
		void SetColorBlendState(const ColorBlendState& state);

		void BindPipelineLayout(PipelineLayout& pipelineLayout);
		void BindBuffer(const Buffer& buffer, uint32_t offset, uint32_t size, uint32_t set, uint32_t binding, uint32_t arrayElement);
		void BindImage(const ImageView& imageView, const Sampler& sampler, uint32_t set, uint32_t binding, uint32_t arrayElement);
		void BindDescriptorSet(VkDescriptorSet descriptorSet);
		void FlushDescriptorSets();

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

		Device& device;
		CommandPool& commandPool;

		PipelineState pipelineState{};
		std::map<uint32_t, BindingMap<VkDescriptorBufferInfo>> bufferBindings;
		std::map<uint32_t, BindingMap<VkDescriptorImageInfo>> imageBindings;
	};
}
