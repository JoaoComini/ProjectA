#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "Core/Window.hpp"

#include "Vulkan/Device.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/Framebuffer.hpp"
#include "Vulkan/Pipeline.hpp"
#include "Vulkan/PipelineLayout.hpp"

#include "Mesh.hpp"
#include "Texture.hpp"
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

	private:
		void CreateDescriptors();
		void CreateRenderPass();
		void CreatePipeline();
		void CreateSampler();
		void CreateFrames();
		void CreateFramebuffers();

		void RecordCommandBuffer(Vulkan::CommandBuffer& commandBuffer);
		bool RecreateSwapchain(bool force = false);

	private:
		uint32_t currentImageIndex = 0;

		const Vulkan::Device& device;
		const Vulkan::Surface& surface;

		std::unique_ptr<Vulkan::Swapchain> swapchain;

		std::vector<std::unique_ptr<Frame>> frames;
		std::vector<std::unique_ptr<Vulkan::Framebuffer>> framebuffers;

		std::unique_ptr<Vulkan::Pipeline> pipeline;
		std::unique_ptr<Vulkan::PipelineLayout> pipelineLayout;

		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		VkSampler sampler;

		VkRenderPass renderPass;

		std::unique_ptr<Mesh> mesh;
		std::unique_ptr<Texture> texture;
	};
}
