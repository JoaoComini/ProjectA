#include "Frame.hpp"

namespace Rendering
{

	Frame::Frame(const Vulkan::Device& device, const Vulkan::DescriptorSetLayout& descriptorSetLayout, std::unique_ptr<Target> target)
		: device(device), descriptorSetLayout(descriptorSetLayout), target(std::move(target))
	{
		commandPool = std::make_unique<Vulkan::CommandPool>(device);
		semaphorePool = std::make_unique<Vulkan::SemaphorePool>(device);
		renderFence = std::make_unique<Vulkan::Fence>(device);

		descriptorPool = std::make_unique<Vulkan::DescriptorPool>(device, descriptorSetLayout, 1000);

		bufferPool = std::make_unique<BufferPool>(device, Vulkan::BufferUsageFlags::UNIFORM);
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

		descriptorPool->Reset();
	}

	Vulkan::CommandBuffer& Frame::RequestCommandBuffer()
	{
		return commandPool->RequestCommandBuffer();
	}

	Vulkan::Semaphore& Frame::RequestSemaphore()
	{
		return semaphorePool->RequestSemaphore();
	}

	VkDescriptorSet Frame::RequestDescriptorSet(BindingMap<VkDescriptorBufferInfo> bufferInfos, BindingMap<VkDescriptorImageInfo> imageInfos)
	{
		auto handle = descriptorPool->Allocate();

		std::vector<VkWriteDescriptorSet> writes;

		for (auto& info : bufferInfos)
		{
			uint32_t index = info.first;
			auto& infos = info.second;

			if (auto binding = descriptorSetLayout.GetBinding(index))
			{
				for (auto& it : infos)
				{
					auto& bufferInfo = it.second;
					writes.push_back(
						{
							.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
							.dstSet = handle,
							.dstBinding = index,
							.descriptorCount = 1,
							.descriptorType = binding->descriptorType,
							.pBufferInfo = &bufferInfo,
						}
					);
				}
			}

			for (auto& info : imageInfos)
			{
				uint32_t index = info.first;
				auto& infos = info.second;

				if (auto binding = descriptorSetLayout.GetBinding(index))
				{
					for (auto& it : infos)
					{
						auto& imageInfo = it.second;
						writes.push_back(
							{
								.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
								.dstSet = handle,
								.dstBinding = index,
								.descriptorCount = 1,
								.descriptorType = binding->descriptorType,
								.pImageInfo = &imageInfo,
							}
						);
					}
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
}