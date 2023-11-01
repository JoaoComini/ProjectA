#pragma once

#include <vulkan/vulkan.h>

#include <vector>
 
#include "Resource.hpp"

namespace Vulkan
{
	class CommandPool;
	class Device;
	class RenderPass;
	class Framebuffer;
	class Pipeline;
	class PipelineLayout;
	class Image;
	class ImageView;

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

		void BeginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer, const std::vector<VkClearValue>& clearValues, VkExtent2D extent);
		void NextSubpass();
		void SetViewport(const std::vector<VkViewport>& viewports);
		void SetScissor(const std::vector<VkRect2D>& scissors);
		void BindPipeline(const Pipeline& pipeline, VkPipelineBindPoint bindPoint);
		void BindDescriptorSet(VkPipelineBindPoint bindPoint, const PipelineLayout& pipelineLayout, uint32_t firstSet, VkDescriptorSet descriptorSet);
		void PushConstants(const PipelineLayout& pipelineLayout, VkShaderStageFlags stages, uint32_t offset, uint32_t size, void* data);
		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

		void EndRenderPass();

		void Free();
		void CopyBuffer(VkBuffer src, VkBuffer dst, uint32_t size);
		void CopyBufferToImage(VkBuffer src, VkImage dst, uint32_t width, uint32_t height);
		void SetImageLayout(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel, uint32_t mipLevels);
		void GenerateMipMaps(const Image& image);

		void ImageMemoryBarrier(const ImageView& imageView, const ImageMemoryBarrierInfo& barrier);

	private:
		const Device& device;
		const CommandPool& commandPool;
	};
}
