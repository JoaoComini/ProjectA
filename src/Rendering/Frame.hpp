#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "Vulkan/Semaphore.hpp"
#include "Vulkan/SemaphorePool.hpp"
#include "Vulkan/Fence.hpp"
#include "Vulkan/CommandPool.hpp"
#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Buffer.hpp"

#include "Target.hpp"

namespace Rendering
{
	struct CameraUniform
	{
		glm::mat4 view;
		glm::mat4 projection;
	};

	class Frame
	{
	public:
		Frame(const Vulkan::Device& device, std::unique_ptr<Target> target);
		~Frame() = default;

		Vulkan::Fence& GetRenderFence() const;

		void Reset();
		Vulkan::CommandBuffer& RequestCommandBuffer();
		Vulkan::Semaphore& RequestSemaphore();

		void SetTarget(std::unique_ptr<Target> target);
		Target& GetTarget() const;

		std::unique_ptr<Vulkan::Buffer> uniformBuffer;
		VkDescriptorSet descriptorSet;

	private:
		const Vulkan::Device& device;

		std::unique_ptr<Vulkan::CommandPool> commandPool;
		std::unique_ptr<Vulkan::SemaphorePool> semaphorePool;
		std::unique_ptr<Vulkan::Fence> renderFence;

		std::unique_ptr<Target> target;
	};
}