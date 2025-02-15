#include "RenderFrame.hpp"

#include "Common/Hash.hpp"

namespace Engine
{

	RenderFrame::RenderFrame(Vulkan::Device& device, std::unique_ptr<RenderTarget> target)
		: device(device), target(std::move(target))
	{
		commandPool = std::make_unique<Vulkan::CommandPool>(device, this);
		semaphorePool = std::make_unique<SemaphorePool>(device);

		renderFence = std::make_unique<Vulkan::Fence>(device);

		bufferPools.emplace(Vulkan::BufferUsageFlags::Uniform, std::make_unique<BufferPool>(device, Vulkan::BufferUsageFlags::Uniform, BUFFER_POOL_BLOCK_SIZE));
		bufferPools.emplace(Vulkan::BufferUsageFlags::Vertex, std::make_unique<BufferPool>(device, Vulkan::BufferUsageFlags::Vertex, BUFFER_POOL_BLOCK_SIZE));
		bufferPools.emplace(Vulkan::BufferUsageFlags::Index, std::make_unique<BufferPool>(device, Vulkan::BufferUsageFlags::Index, BUFFER_POOL_BLOCK_SIZE));
	}

	void RenderFrame::Reset()
	{
		renderFence->Wait();
		renderFence->Reset();

		commandPool->Reset();
		semaphorePool->Reset();

		for (auto& [_, pool]:descriptorPools)
		{
			pool->Reset();
		}

		for (auto& [_, pool] : bufferPools)
		{
			pool->Reset();
		}
	}

	Vulkan::CommandBuffer& RenderFrame::RequestCommandBuffer()
	{
		return commandPool->RequestCommandBuffer();
	}

	Vulkan::Semaphore& RenderFrame::RequestSemaphore()
	{
		return semaphorePool->RequestSemaphore();
	}

	Vulkan::Semaphore* RenderFrame::RequestOwnedSemaphore()
	{
		return semaphorePool->RequestOwnedSemaphore();
	}

	void RenderFrame::ReleaseOwnedSemaphore(Vulkan::Semaphore* semaphore)
	{
		semaphorePool->ReleaseOwnedSemaphore(semaphore);
	}

	Vulkan::Fence& RenderFrame::GetRenderFence() const
	{
		return *renderFence;
	}

	VkDescriptorSet RenderFrame::RequestDescriptorSet(Vulkan::DescriptorSetLayout& descriptorSetLayout, const BindingMap<VkDescriptorBufferInfo>& bufferInfos, const BindingMap<VkDescriptorImageInfo>& imageInfos)
	{
		auto handle = GetDescriptorPool(descriptorSetLayout).Allocate();

		thread_local std::vector<VkWriteDescriptorSet> writes;
		writes.clear();

		for (auto& [index, infos] : bufferInfos)
		{
			if (auto layoutBinding = descriptorSetLayout.GetBinding(index))
			{
				for (auto& [element, info] : infos)
				{
					writes.push_back(
						{
							.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
							.dstSet = handle,
							.dstBinding = index,
							.dstArrayElement = element,
							.descriptorCount = 1,
							.descriptorType = layoutBinding->descriptorType,
							.pBufferInfo = &info,
						}
					);
				}
			}
		}

		for (auto& [index, infos] : imageInfos)
		{
			if (auto layoutBinding = descriptorSetLayout.GetBinding(index))
			{
				for (auto& [element, info] : infos)
				{
					writes.push_back(
						{
							.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
							.dstSet = handle,
							.dstBinding = index,
							.dstArrayElement = element,
							.descriptorCount = 1,
							.descriptorType = layoutBinding->descriptorType,
							.pImageInfo = &info,
						}
					);
				}
			}
		}

		vkUpdateDescriptorSets(device.GetHandle(), writes.size(), writes.data(), 0, nullptr);

		return handle;
	}

	BufferAllocation RenderFrame::RequestBufferAllocation(Vulkan::BufferUsageFlags usage, uint32_t size)
	{
		auto it = bufferPools.find(usage);

		if (it == bufferPools.end())
		{
			return {};
		}

		auto& pool = it->second;

		return pool->Allocate(size);
	}

	void RenderFrame::SetTarget(std::unique_ptr<RenderTarget> target)
	{
		this->target = std::move(target);
	}

	RenderTarget& RenderFrame::GetTarget() const
	{
		return *target;
	}

	DescriptorPool& RenderFrame::GetDescriptorPool(Vulkan::DescriptorSetLayout& descriptorSetLayout)
	{
		std::size_t hash{ 0 };
		Hash(hash, descriptorSetLayout);

		auto it = descriptorPools.find(hash);

		if (it != descriptorPools.end())
		{
			return *it->second;
		}

		auto [res, _] = descriptorPools.emplace(hash, std::make_unique<DescriptorPool>(device, descriptorSetLayout, DESCRIPTOR_POOL_MAX_SETS));

		return *res->second;
	}
}