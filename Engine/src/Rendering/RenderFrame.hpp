#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include <unordered_map>
#include <map>

#include "Vulkan/Semaphore.hpp"
#include "Vulkan/Fence.hpp"
#include "Vulkan/CommandPool.hpp"
#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/DescriptorSetLayout.hpp"
#include "Vulkan/Buffer.hpp"

#include "RenderTarget.hpp"
#include "Pool/BufferPool.hpp"
#include "Pool/DescriptorPool.hpp"
#include "Pool/SemaphorePool.hpp"
#include "Caching/FramebufferCache.hpp"

namespace Engine
{
	template <typename T>
	using BindingMap = std::map<uint32_t, std::map<uint32_t, T>>;

	class RenderFrame
	{
	public:
		static constexpr uint32_t BUFFER_POOL_BLOCK_SIZE = 256 * 1024;
		static constexpr uint32_t DESCRIPTOR_POOL_MAX_SETS = 256;

		RenderFrame(const Vulkan::Device& device, std::unique_ptr<RenderTarget> target);
		~RenderFrame() = default;

		void Reset();

		Vulkan::CommandBuffer& RequestCommandBuffer();
		Vulkan::Semaphore& RequestSemaphore();
		Vulkan::Fence& GetRenderFence() const;
		Vulkan::Framebuffer& RequestFramebuffer(const Vulkan::RenderPass& renderPass);
		void ClearFramebuffers();

		VkDescriptorSet RequestDescriptorSet(Vulkan::DescriptorSetLayout& descriptorSetLayout, const BindingMap<VkDescriptorBufferInfo>& bufferInfos, const BindingMap<VkDescriptorImageInfo>& imageInfos);
		BufferAllocation RequestBufferAllocation(Vulkan::BufferUsageFlags usage, uint32_t size);

		void SetTarget(std::unique_ptr<RenderTarget> target);
		RenderTarget& GetTarget() const;
	private:
		DescriptorPool& GetDescriptorPool(Vulkan::DescriptorSetLayout& descriptorSetLayout);

		const Vulkan::Device& device;

		std::unique_ptr<Vulkan::CommandPool> commandPool;
		std::unique_ptr<SemaphorePool> semaphorePool;
		std::unique_ptr<BufferPool> bufferPool;
		std::unique_ptr<FramebufferCache> framebufferCache;

		std::unordered_map<std::size_t, std::unique_ptr<DescriptorPool>> descriptorPools;

		std::unique_ptr<Vulkan::Fence> renderFence;

		std::unique_ptr<RenderTarget> target;
	};
}