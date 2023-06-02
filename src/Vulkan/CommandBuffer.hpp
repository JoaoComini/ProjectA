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
		void SetViewport(const std::vector<VkViewport>& viewports);
		void SetScissor(const std::vector<VkRect2D>& scissors);
		void BindPipeline(const Pipeline& pipeline, VkPipelineBindPoint bindPoint);
		void BindDescriptorSet(VkPipelineBindPoint bindPoint, const PipelineLayout& pipelineLayout, uint32_t firstSet, VkDescriptorSet descriptorSet);
		void EndRenderPass();

		void Free();
		void CopyBuffer(VkBuffer src, VkBuffer dst, uint32_t size);
		void CopyBufferToImage(VkBuffer src, VkImage dst, uint32_t width, uint32_t height);
		void SetImageLayout(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel, uint32_t mipLevels);
		void GenerateMipMaps(const Image& image);

	private:
		const Device& device;
		const CommandPool& commandPool;
	};
}
