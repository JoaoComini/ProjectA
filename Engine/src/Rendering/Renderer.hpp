#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "Core/Window.hpp"

#include "Vulkan/Device.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/Framebuffer.hpp"
#include "Vulkan/Pipeline.hpp"
#include "Vulkan/PipelineLayout.hpp"
#include "Vulkan/RenderPass.hpp"
#include "Vulkan/DescriptorSetLayout.hpp"

#include "Common/Singleton.hpp"

#include "Mesh.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Camera.hpp"
#include "Frame.hpp"

namespace Engine
{
	class Renderer : public Singleton<Renderer>
	{
	public:
		Renderer(Vulkan::Device& device, std::unique_ptr<Vulkan::Swapchain> swapchain);

		static void Setup(Vulkan::Device& device, const Vulkan::Surface& surface, const Window& window);

		Vulkan::CommandBuffer* Begin();
		void End(Vulkan::CommandBuffer& commandBuffer);

		Vulkan::RenderPass& GetRenderPass() const;
		Frame& GetCurrentFrame() const;

	private:
		void CreateFrames();
		void BeginRenderPass(Vulkan::CommandBuffer& commandBuffer);
		Vulkan::Semaphore& Submit(Vulkan::CommandBuffer& commandBuffer);
		void Present(Vulkan::Semaphore& waitSemaphore);

		bool RecreateSwapchain(bool force = false);

		std::unique_ptr<Target> CreateTarget(VkImage image, VkFormat format, VkExtent2D extent);

		const Vulkan::Device& device;

		uint32_t currentImageIndex = 0;

		std::unique_ptr<Vulkan::Swapchain> swapchain;
		Vulkan::Semaphore* acquireSemaphore;

		std::vector<std::unique_ptr<Frame>> frames;

		std::unique_ptr<Vulkan::RenderPass> renderPass;
	};
}
