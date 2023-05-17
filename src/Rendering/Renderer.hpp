#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "Core/Window.hpp"

#include "Vulkan/Device.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/CommandBuffer.hpp"

#include "Mesh.hpp"
#include "Frame.hpp"

namespace Rendering
{
	struct ModelConstant
	{
		glm::mat4 model;
	};

	class Renderer
	{
	public:
		Renderer(Vulkan::Device& device, const Vulkan::Surface& surface, const Window& window);
		~Renderer();

		void Render();

		void ResetImages();
		void CreateImages();

	private:
		void CreateDescriptors();
		void CreateRenderPass();
		void CreatePipeline();
		void CreateFramebuffers();
		void RecordCommandBuffer(Vulkan::CommandBuffer& commandBuffer);
		bool RecreateSwapchain(bool force = false);

	private:
		uint32_t currentImageIndex = 0;

		const Vulkan::Device& device;
		const Vulkan::Surface& surface;

		std::unique_ptr<Vulkan::Swapchain> swapchain;
		std::unique_ptr<Vulkan::Image> depthImage;

		std::vector<std::unique_ptr<Frame>> frames;

		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;

		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		std::vector<VkFramebuffer> framebuffers;

		std::unique_ptr<Mesh> mesh;
	};
}
