#include "CommandBuffer.hpp"

#include "Device.hpp"
#include "CommandPool.hpp"
#include "RenderPass.hpp"
#include "Framebuffer.hpp"
#include "Pipeline.hpp"
#include "ImageView.hpp"
#include "Caching/ResourceCache.hpp"

namespace Vulkan
{
	CommandBuffer::CommandBuffer(const Device& device, const CommandPool& commandPool, const Level level) : device(device), commandPool(commandPool)
	{
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = commandPool.GetHandle();
		allocateInfo.level = level == Level::Primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocateInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device.GetHandle(), &allocateInfo, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	CommandBuffer::~CommandBuffer()
	{
		if (handle == VK_NULL_HANDLE)
		{
			return;
		}

		vkFreeCommandBuffers(device.GetHandle(), commandPool.GetHandle(), 1, &handle);
	}

	void CommandBuffer::Begin(BeginFlags flags)
	{
		pipelineState.Reset();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = static_cast<VkFlags>(flags);                  // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(handle, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}

	void CommandBuffer::End()
	{
		vkEndCommandBuffer(handle);
	}

	void CommandBuffer::BeginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer, const std::vector<VkClearValue>& clearValues, VkExtent2D extent)
	{
		pipelineState.Reset();

		pipelineState.SetRenderPass(renderPass);

		VkRenderPassBeginInfo info{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = renderPass.GetHandle(),
			.framebuffer = framebuffer.GetHandle(),
			.renderArea = {
				.extent = extent,
			},
			.clearValueCount = static_cast<uint32_t>(clearValues.size()),
			.pClearValues = clearValues.data()
		};

		vkCmdBeginRenderPass(handle, &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	void CommandBuffer::NextSubpass()
	{
		pipelineState.SetSubpassIndex(pipelineState.GetSubpassIndex() + 1);

		vkCmdNextSubpass(handle, VK_SUBPASS_CONTENTS_INLINE);
	}

	void CommandBuffer::SetViewport(const std::vector<VkViewport>& viewports)
	{
		vkCmdSetViewport(handle, 0, viewports.size(), viewports.data());
	}

	void CommandBuffer::SetScissor(const std::vector<VkRect2D>& scissors)
	{
		vkCmdSetScissor(handle, 0, scissors.size(), scissors.data());
	}

	void CommandBuffer::SetVertexInputState(const VertexInputState& state)
	{
		pipelineState.SetVertexInputState(state);
	}

	void CommandBuffer::SetMultisampleState(const MultisampleState& state)
	{
		pipelineState.SetMultisampleState(state);
	}

	void CommandBuffer::SetInputAssemblyState(const InputAssemblyState& state)
	{
		pipelineState.SetInputAssemblyState(state);
	}

	void CommandBuffer::SetRasterizationState(const RasterizationState& state)
	{
		pipelineState.SetRasterizationState(state);
	}

	void CommandBuffer::SetDepthStencilState(const DepthStencilState& state)
	{
		pipelineState.SetDepthStencilState(state);
	}

	void CommandBuffer::BindPipelineLayout(PipelineLayout& pipelineLayout)
	{
		pipelineState.SetPipelineLayout(pipelineLayout);
	}

	void CommandBuffer::BindDescriptorSet(VkDescriptorSet descriptorSet)
	{
		vkCmdBindDescriptorSets(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineState.GetPipelineLayout()->GetHandle(), 0, 1, &descriptorSet, 0, nullptr);
	}

	void CommandBuffer::PushConstants(VkShaderStageFlags stages, uint32_t offset, uint32_t size, void* data)
	{
		vkCmdPushConstants(handle, pipelineState.GetPipelineLayout()->GetHandle(), stages, offset, size, data);
	}

	void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		Flush();

		vkCmdDraw(handle, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
	{
		Flush();

		vkCmdDrawIndexed(handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void CommandBuffer::EndRenderPass()
	{
		vkCmdEndRenderPass(handle);
	}

	void CommandBuffer::Free()
	{
		vkFreeCommandBuffers(device.GetHandle(), commandPool.GetHandle(), 1, &handle);
	}

	void CommandBuffer::CopyBuffer(VkBuffer src, VkBuffer dst, uint32_t size)
	{
		VkBufferCopy copy{
			.size = size
		};

		vkCmdCopyBuffer(handle, src, dst, 1, &copy);
	}

	void CommandBuffer::CopyBufferToImage(VkBuffer src, VkImage dst, uint32_t width, uint32_t height)
	{
		VkBufferImageCopy copy{
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
			.imageOffset = { 0, 0, 0 },
			.imageExtent = {
				width,
				height,
				1
			}
		};

		vkCmdCopyBufferToImage(handle, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
	}

	void CommandBuffer::SetImageLayout(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel, uint32_t mipLevels)
	{

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image.GetHandle();
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = baseMipLevel;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage{};
		VkPipelineStageFlags destinationStage{};

		switch (oldLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			// Image layout is undefined (or does not matter)
			// Only valid as initial layout
			// No flags required, listed only for completeness
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image is a transfer destination
			// Make sure any writes to the image have been finished
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image is a transfer source
			// Make sure any reads from the image have been finished
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
		}

		switch (newLayout)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image will be used as a transfer destination
			// Make sure any writes to the image have been finished
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image will be used as a transfer source
			// Make sure any reads from the image have been finished
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image will be read in a shader (sampler, input attachment)
			// Make sure any writes to the image have been finished
			if (barrier.srcAccessMask == 0)
			{
				barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
		}

		vkCmdPipelineBarrier(
			handle,
			sourceStage,
			destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}

	void CommandBuffer::GenerateMipMaps(const Image& image)
	{
		VkExtent3D extent = image.GetExtent();

		int width = extent.width;
		int height = extent.height;

		for (uint32_t i = 1; i < image.GetMipLevels(); i++)
		{
			SetImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, i - 1, 1);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { width, height, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { width > 1 ? width / 2 : 1, height > 1 ? height / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(
				handle,
				image.GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image.GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR
			);

			SetImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i - 1, 1);

			width = width > 1 ? width / 2 : width;
			height = height > 1 ? height / 2 : height;
		}

		SetImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, image.GetMipLevels() - 1, 1);
	}

	void CommandBuffer::ImageMemoryBarrier(const ImageView& imageView, const ImageMemoryBarrierInfo& barrierInfo)
	{
		auto& image = imageView.GetImage();

		auto subresourceRange = imageView.GetSubresourceRange();
		auto format = image.GetFormat();

		subresourceRange.aspectMask = format == VK_FORMAT_D32_SFLOAT
			? VK_IMAGE_ASPECT_DEPTH_BIT
			: VK_IMAGE_ASPECT_COLOR_BIT;

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcAccessMask = barrierInfo.srcAccessMask;
		barrier.dstAccessMask = barrierInfo.dstAccessMask;
		barrier.oldLayout = barrierInfo.oldLayout;
		barrier.newLayout = barrierInfo.newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image.GetHandle();
		barrier.subresourceRange = subresourceRange;

		vkCmdPipelineBarrier(handle, barrierInfo.srcStageMask, barrierInfo.dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}

	void CommandBuffer::Flush()
	{
		if (!pipelineState.IsDirty())
		{
			return;
		}

		pipelineState.ClearDirty();

		auto& pipeline = device.GetResourceCache().RequestPipeline(pipelineState);

		vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetHandle());
	}
}
