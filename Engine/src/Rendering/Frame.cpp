#include "Frame.hpp"

#include "Common/Hash.hpp"

namespace Engine
{

	Frame::Frame(const Vulkan::Device& device, std::unique_ptr<Target> target)
		: device(device), target(std::move(target))
	{
		commandPool = std::make_unique<Vulkan::CommandPool>(device);
		semaphorePool = std::make_unique<SemaphorePool>(device);
		framebufferCache = std::make_unique<FramebufferCache>(device);

		renderFence = std::make_unique<Vulkan::Fence>(device);

		bufferPool = std::make_unique<BufferPool>(device, Vulkan::BufferUsageFlags::UNIFORM, BUFFER_POOL_BLOCK_SIZE);
	}

	Vulkan::Fence& Frame::GetRenderFence() const
	{
		return *renderFence;
	}

	void Frame::Reset()
	{
		renderFence->Wait();
		renderFence->Reset();

		commandPool->Reset();
		semaphorePool->Reset();

		bufferPool->Reset();

		for (auto& [_, pool]:descriptorPools)
		{
			pool->Reset();
		}
	}

	Vulkan::CommandBuffer& Frame::RequestCommandBuffer()
	{
		return commandPool->RequestCommandBuffer();
	}

	Vulkan::Semaphore& Frame::RequestSemaphore()
	{
		return semaphorePool->RequestSemaphore();
	}

	Vulkan::Framebuffer& Frame::RequestFramebuffer(const Vulkan::RenderPass& renderPass)
	{
		return framebufferCache->RequestFramebuffer(renderPass, *target);
	}

	VkDescriptorSet Frame::RequestDescriptorSet(Vulkan::DescriptorSetLayout& descriptorSetLayout, const std::vector<SetBinding<VkDescriptorBufferInfo>>& bufferInfos, const std::vector<SetBinding<VkDescriptorImageInfo>>& imageInfos)
	{
		auto handle = GetDescriptorPool(descriptorSetLayout).Allocate();

		std::vector<VkWriteDescriptorSet> writes;

		for (auto& info : bufferInfos)
		{
			uint32_t index = info.binding;
			if (auto binding = descriptorSetLayout.GetBinding(index))
			{
				for (uint32_t i = 0; i < info.infos.size(); i++)
				{
					writes.push_back(
						{
							.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
							.dstSet = handle,
							.dstBinding = index,
							.dstArrayElement = i,
							.descriptorCount = 1,
							.descriptorType = binding->descriptorType,
							.pBufferInfo = &info.infos[i],
						}
					);
				}
			}
		}

		for (auto& info : imageInfos)
		{
			uint32_t index = info.binding;
			if (auto binding = descriptorSetLayout.GetBinding(index))
			{
				for (uint32_t i = 0; i < info.infos.size(); i++)
				{
					writes.push_back(
						{
							.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
							.dstSet = handle,
							.dstBinding = index,
							.dstArrayElement = i,
							.descriptorCount = 1,
							.descriptorType = binding->descriptorType,
							.pImageInfo = &info.infos[i],
						}
					);
				}
			}
		}

		vkUpdateDescriptorSets(device.GetHandle(), writes.size(), writes.data(), 0, nullptr);

		return handle;
	}

	BufferAllocation Frame::RequestBufferAllocation(Vulkan::BufferUsageFlags usage, uint32_t size)
	{
		return bufferPool->Allocate(size);
	}

	void Frame::SetTarget(std::unique_ptr<Target> target)
	{
		this->target = std::move(target);
	}

	Target& Frame::GetTarget() const
	{
		return *target;
	}

	DescriptorPool& Frame::GetDescriptorPool(Vulkan::DescriptorSetLayout& descriptorSetLayout)
	{
		std::size_t hash = Hash(descriptorSetLayout);

		auto it = descriptorPools.find(hash);

		if (it != descriptorPools.end())
		{
			return *it->second;
		}

		auto [res, _] = descriptorPools.emplace(hash, std::make_unique<DescriptorPool>(device, descriptorSetLayout, DESCRIPTOR_POOL_MAX_SETS));

		return *res->second;
	}
}