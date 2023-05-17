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
		Frame(Vulkan::Device& device);
		~Frame() = default;

		Vulkan::Fence& GetRenderFence() const;

		void Reset();
		Vulkan::CommandBuffer& RequestCommandBuffer();
		Vulkan::Semaphore& RequestSemaphore();


		std::unique_ptr<Vulkan::Buffer> uniformBuffer;
		VkDescriptorSet descriptorSet;

	private:
		Vulkan::Device& device;

		std::unique_ptr<Vulkan::CommandPool> commandPool;
		std::unique_ptr<Vulkan::SemaphorePool> semaphorePool;
		std::unique_ptr<Vulkan::Fence> renderFence;
	};
}