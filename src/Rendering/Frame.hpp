#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include <map>

#include "Vulkan/Semaphore.hpp"
#include "Vulkan/Fence.hpp"
#include "Vulkan/CommandPool.hpp"
#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/DescriptorSetLayout.hpp"
#include "Vulkan/Buffer.hpp"

#include "Target.hpp"
#include "BufferPool.hpp"
#include "DescriptorPool.hpp"
#include "SemaphorePool.hpp"

namespace Rendering
{
	template <class T>
	using BindingMap = std::map<uint32_t, std::map<uint32_t, T>>;

	struct GlobalUniform
	{
		glm::mat4 viewProjection;
		glm::mat4 model;
	};

	class Frame
	{
	public:
		static constexpr uint32_t BUFFER_POOL_BLOCK_SIZE = 256 * 1024;
		static constexpr uint32_t DESCRIPTOR_POOL_MAX_SETS = 256;

		Frame(const Vulkan::Device& device, const Vulkan::DescriptorSetLayout& descriptorSetLayout, std::unique_ptr<Target> target);
		~Frame() = default;

		Vulkan::Fence& GetRenderFence() const;

		void Reset();
		Vulkan::CommandBuffer& RequestCommandBuffer();
		Vulkan::Semaphore& RequestSemaphore();
		VkDescriptorSet RequestDescriptorSet(BindingMap<VkDescriptorBufferInfo> bufferInfos, BindingMap<VkDescriptorImageInfo> imageInfos);

		BufferAllocation RequestBufferAllocation(Vulkan::BufferUsageFlags usage, uint32_t size);

		void SetTarget(std::unique_ptr<Target> target);
		Target& GetTarget() const;
	private:
		const Vulkan::Device& device;
		const Vulkan::DescriptorSetLayout& descriptorSetLayout;

		std::unique_ptr<Vulkan::CommandPool> commandPool;
		std::unique_ptr<SemaphorePool> semaphorePool;
		std::unique_ptr<DescriptorPool> descriptorPool;

		std::unique_ptr<Vulkan::Fence> renderFence;

		std::unique_ptr<Target> target;

		std::unique_ptr<BufferPool> bufferPool;
	};
}