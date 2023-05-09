#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "../Vulkan/Device.hpp"
#include "../Vulkan/Swapchain.hpp"
#include "../Vulkan/Image.hpp"
#include "../Vulkan/Buffer.hpp"
#include "Mesh.hpp"

struct GlobalUniform
{
	glm::mat4 mvp;
};

class Renderer
{
public:
	Renderer(Vulkan::Device& device, const Vulkan::Swapchain& swapchain);
	~Renderer();

	void Render(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void ResetImages();
	void CreateImages();

private:
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	std::vector<VkFramebuffer> framebuffers;

	const Vulkan::Device& device;
	const Vulkan::Swapchain& swapchain;
	std::unique_ptr<Vulkan::Image> depthImage;

	std::unique_ptr<Mesh> mesh;

	void CreateRenderPass();
	void CreatePipeline();
	void CreateFramebuffers();

	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

};


