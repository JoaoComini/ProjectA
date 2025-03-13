#pragma once

#include "Vulkan/Semaphore.h"
#include "Vulkan/Fence.h"
#include "Vulkan/CommandPool.h"
#include "Vulkan/CommandBuffer.h"
#include "Vulkan/Device.h"
#include "Vulkan/Buffer.h"
#include "Vulkan/DescriptorSetLayout.h"
#include "Vulkan/Buffer.h"

#include "Pool/BufferPool.h"
#include "Pool/DescriptorPool.h"
#include "Pool/SemaphorePool.h"

#include "RenderTarget.h"

namespace Engine
{
	template <typename T>
	using BindingMap = std::map<uint32_t, std::map<uint32_t, T>>;

	class RenderFrame
	{
	public:
		static constexpr uint32_t BUFFER_POOL_BLOCK_SIZE = 256 * 1024;
		static constexpr uint32_t DESCRIPTOR_POOL_MAX_SETS = 256;

		RenderFrame(Vulkan::Device& device, std::unique_ptr<RenderTarget> target);
		~RenderFrame() = default;

		void Reset();

		Vulkan::CommandBuffer& RequestCommandBuffer();
		Vulkan::Semaphore& RequestSemaphore();
		Vulkan::Semaphore* RequestOwnedSemaphore();
		void ReleaseOwnedSemaphore(Vulkan::Semaphore* semaphore);
		Vulkan::Fence& GetRenderFence() const;

		VkDescriptorSet RequestDescriptorSet(Vulkan::DescriptorSetLayout& descriptorSetLayout, const BindingMap<VkDescriptorBufferInfo>& bufferInfos, const BindingMap<VkDescriptorImageInfo>& imageInfos);
		BufferAllocation RequestBufferAllocation(Vulkan::BufferUsageFlags usage, uint32_t size);

		void SetTarget(std::unique_ptr<RenderTarget> target);
		RenderTarget& GetTarget() const;
	private:
		DescriptorPool& GetDescriptorPool(Vulkan::DescriptorSetLayout& descriptorSetLayout);

		Vulkan::Device& device;

		std::unique_ptr<Vulkan::CommandPool> commandPool;
		std::unique_ptr<SemaphorePool> semaphorePool;

		std::unordered_map<Vulkan::BufferUsageFlags, std::unique_ptr<BufferPool>> bufferPools;
		std::unordered_map<std::size_t, std::unique_ptr<DescriptorPool>> descriptorPools;

		std::unique_ptr<Vulkan::Fence> renderFence;

		std::unique_ptr<RenderTarget> target;
	};
}