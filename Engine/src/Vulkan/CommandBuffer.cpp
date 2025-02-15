#include "CommandBuffer.hpp"

#include "Device.hpp"
#include "CommandPool.hpp"
#include "Pipeline.hpp"
#include "ImageView.hpp"
#include "Sampler.hpp"
#include "ResourceCache.hpp"
#include "Rendering/RenderFrame.hpp"

namespace Vulkan
{
	CommandBuffer::CommandBuffer(Device& device, CommandPool& commandPool, Level level) : device(device), commandPool(commandPool)
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
		beginInfo.flags = static_cast<VkFlags>(flags);
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(handle, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}

	void CommandBuffer::End()
	{
		vkEndCommandBuffer(handle);
	}

	void CommandBuffer::BeginRendering(const VkRenderingInfo& renderingInfo)
	{
		vkCmdBeginRendering(handle, &renderingInfo);
	}

	void CommandBuffer::EndRendering()
	{
		vkCmdEndRendering(handle);

		imageBindings.clear();
		bufferBindings.clear();
		pipelineState.Reset();
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

	void CommandBuffer::SetPipelineRenderingState(const PipelineRenderingState& state)
	{
		pipelineState.SetPipelineRenderingState(state);
	}

	void CommandBuffer::SetColorBlendState(const ColorBlendState& state)
	{
		pipelineState.SetColorBlendState(state);
	}

	void CommandBuffer::BindPipelineLayout(PipelineLayout& pipelineLayout)
	{
		pipelineState.SetPipelineLayout(pipelineLayout);
	}

	void CommandBuffer::Flush()
	{
		FlushDescriptorSets();

		if (!pipelineState.IsDirty())
		{
			return;
		}

		pipelineState.ClearDirty();

		auto& pipeline = device.GetResourceCache().RequestPipeline(pipelineState);

		vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetHandle());
	}

	void CommandBuffer::BindDescriptorSet(VkDescriptorSet descriptorSet)
	{
		vkCmdBindDescriptorSets(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineState.GetPipelineLayout()->GetHandle(), 0, 1, &descriptorSet, 0, nullptr);
	}

	void CommandBuffer::PushConstants(VkShaderStageFlags stages, uint32_t offset, uint32_t size, void* data)
	{
		vkCmdPushConstants(handle, pipelineState.GetPipelineLayout()->GetHandle(), stages, offset, size, data);
	}

	void CommandBuffer::BindBuffer(const Buffer& buffer, uint32_t offset, uint32_t size, uint32_t set, uint32_t binding, uint32_t arrayElement)
	{
		bufferBindings[set][binding][arrayElement] = { buffer.GetHandle(), offset, size };

	}

	void CommandBuffer::BindImage(const ImageView& imageView, const Sampler& sampler, uint32_t set, uint32_t binding, uint32_t arrayElement)
	{
		imageBindings[set][binding][arrayElement] = { sampler.GetHandle(), imageView.GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	}

	void CommandBuffer::FlushDescriptorSets()
	{
		assert(pipelineState.GetPipelineLayout());

		for (int set = 0; set < std::max(bufferBindings.size(), imageBindings.size()); set++)
		{
			auto* layout = pipelineState.GetPipelineLayout();
			auto& descritorSetLayout = layout->GetDescriptorSetLayout(set);

			auto* frame = commandPool.GetRenderFrame();
			auto descriptorSet = frame->RequestDescriptorSet(descritorSetLayout, bufferBindings[set], imageBindings[set]);

			BindDescriptorSet(descriptorSet);
		}
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

	void CommandBuffer::CopyBufferToImage(const Buffer& buffer, const Image& image, const std::vector<VkBufferImageCopy>& regions)
	{
		vkCmdCopyBufferToImage(handle, buffer.GetHandle(), image.GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(), regions.data());
	}

	void CommandBuffer::SetImageLayout(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageSubresourceRange subresourceRange)
	{
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image.GetHandle();
		barrier.subresourceRange = subresourceRange;

		switch (oldLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			// Image layout is undefined (or does not matter)
			// Only valid as initial layout
			// No flags required, listed only for completeness
			barrier.srcAccessMask = 0;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image is a transfer destination
			// Make sure any writes to the image have been finished
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image is a transfer source
			// Make sure any reads from the image have been finished
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
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
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image will be used as a transfer source
			// Make sure any reads from the image have been finished
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image will be read in a shader (sampler, input attachment)
			// Make sure any writes to the image have been finished
			if (barrier.srcAccessMask == 0)
			{
				barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
		}

		vkCmdPipelineBarrier(
			handle,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}

	void CommandBuffer::GenerateMipMaps(const Image& image)
	{
		VkExtent3D extent = image.GetExtent();

		for (uint32_t layer = 0; layer < image.GetArrayLayers(); layer++)
		{
			int width = extent.width;
			int height = extent.height;

			for (uint32_t level = 1; level < image.GetMipLevels(); level++)
			{
				VkImageSubresourceRange subresourceRange{};
				subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				subresourceRange.baseArrayLayer = layer;
				subresourceRange.layerCount = 1;
				subresourceRange.baseMipLevel = level - 1;
				subresourceRange.levelCount = 1;

				SetImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, subresourceRange);

				VkImageBlit blit{};
				blit.srcOffsets[0] = { 0, 0, 0 };
				blit.srcOffsets[1] = { width, height, 1 };
				blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel = level - 1;
				blit.srcSubresource.baseArrayLayer = layer;
				blit.srcSubresource.layerCount = 1;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { width > 1 ? width / 2 : 1, height > 1 ? height / 2 : 1, 1 };
				blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel = level;
				blit.dstSubresource.baseArrayLayer = layer;
				blit.dstSubresource.layerCount = 1;

				vkCmdBlitImage(
					handle,
					image.GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					image.GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR
				);

				SetImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);

				width = width > 1 ? width / 2 : width;
				height = height > 1 ? height / 2 : height;
			}

			VkImageSubresourceRange subresourceRange{};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseArrayLayer = layer;
			subresourceRange.layerCount = 1;
			subresourceRange.baseMipLevel = image.GetMipLevels() - 1;
			subresourceRange.levelCount = 1;

			SetImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
		}
	}

	void CommandBuffer::ImageMemoryBarrier(const ImageView& imageView, const ImageMemoryBarrierInfo& barrierInfo)
	{
		auto& image = imageView.GetImage();

		auto subresourceRange = imageView.GetSubresourceRange();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcAccessMask = barrierInfo.srcAccessMask;
		barrier.dstAccessMask = barrierInfo.dstAccessMask;
		barrier.oldLayout = barrierInfo.oldLayout;
		barrier.newLayout = barrierInfo.newLayout;
		barrier.srcQueueFamilyIndex = barrierInfo.oldQueueFamily;
		barrier.dstQueueFamilyIndex = barrierInfo.newQueueFamily;
		barrier.image = image.GetHandle();
		barrier.subresourceRange = subresourceRange;

		vkCmdPipelineBarrier(handle, barrierInfo.srcStageMask, barrierInfo.dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}
}
