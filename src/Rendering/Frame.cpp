#include "Frame.hpp"

namespace Rendering
{

	Frame::Frame(const Vulkan::Device& device, const Vulkan::DescriptorSetLayout& descriptorSetLayout, std::unique_ptr<Target> target)
		: device(device), descriptorSetLayout(descriptorSetLayout), target(std::move(target))
	{
		commandPool = std::make_unique<Vulkan::CommandPool>(device);
		semaphorePool = std::make_unique<Vulkan::SemaphorePool>(device);
		renderFence = std::make_unique<Vulkan::Fence>(device);

		uniformBuffer = Vulkan::BufferBuilder()
			.Persistent()
			.AllowTransfer()
			.SequentialWrite()
			.BufferUsage(Vulkan::BufferUsageFlags::UNIFORM)
			.Size(sizeof(GlobalUniform))
			.Build(device);

		descriptorPool = std::make_unique<Vulkan::DescriptorPool>(device, descriptorSetLayout, 10);
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

		buffers.clear();

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

	Vulkan::Buffer& Frame::RequestBuffer(Vulkan::BufferUsageFlags usage, uint32_t size)
	{
		auto buffer = Vulkan::BufferBuilder()
			.BufferUsage(usage)
			.Persistent()
			.SequentialWrite()
			.Size(size)
			.Build(device);

		buffers.push_back(std::move(buffer));

		return *buffers.back();
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