#include "Renderer.hpp"


#include <array>

#include "Vulkan/Shader.hpp"
#include "Vertex.hpp"


namespace Rendering
{

	Renderer::Renderer(Vulkan::Device& device, const Vulkan::Surface& surface, const Window& window) : device(device), surface(surface)
	{
		mesh = std::make_unique<Mesh>(device, "resources/models/viking_room.obj");
		texture = std::make_unique<Texture>(device, "resources/models/viking_room.png");

		auto size = window.GetFramebufferSize();

		swapchain = Vulkan::SwapchainBuilder()
			.DesiredWidth(size.width)
			.DesiredHeight(size.height)
			.MinImageCount(3)
			.Build(device, surface);

		renderPass = std::make_unique<Vulkan::RenderPass>(device, *swapchain);

		CreateDescriptors();
		CreatePipeline();
		CreateSampler();
		CreateFrames();
		CreateFramebuffers();
	}

	Renderer::~Renderer()
	{
		vkDestroyDescriptorSetLayout(device.GetHandle(), descriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(device.GetHandle(), descriptorPool, nullptr);
		vkDestroySampler(device.GetHandle(), sampler, nullptr);
	}

	void Renderer::CreateDescriptors()
	{
		VkDescriptorSetLayoutBinding uniformBufferBinding = {
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
		};

		VkDescriptorSetLayoutBinding samplerBinding = {
			.binding = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			.pImmutableSamplers = nullptr,
		};

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uniformBufferBinding, samplerBinding };

		VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = bindings.size(),
			.pBindings = bindings.data()
		};

		vkCreateDescriptorSetLayout(device.GetHandle(), &layoutCreateInfo, nullptr, &descriptorSetLayout);

		std::vector<VkDescriptorPoolSize> sizes =
		{
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
		};

		VkDescriptorPoolCreateInfo poolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = 10,
			.poolSizeCount = (uint32_t)sizes.size(),
			.pPoolSizes = sizes.data()
		};

		vkCreateDescriptorPool(device.GetHandle(), &poolCreateInfo, nullptr, &descriptorPool);
	}

	void Renderer::CreatePipeline()
	{

		pipelineLayout = std::make_unique<Vulkan::PipelineLayout>(device, std::vector<VkDescriptorSetLayout>{descriptorSetLayout});

		auto spec = Vulkan::PipelineSpec
		{
			.vertexInputSpec
			{
				.attributes = {
					VkVertexInputAttributeDescription{
						.location = 0,
						.binding = 0,
						.format = VK_FORMAT_R32G32B32_SFLOAT,
						.offset = offsetof(Vertex, position),
					},
					VkVertexInputAttributeDescription{
						.location = 1,
						.binding = 0,
						.format = VK_FORMAT_R32G32B32_SFLOAT,
						.offset = offsetof(Vertex, normal),
					},
					VkVertexInputAttributeDescription{
						.location = 2,
						.binding = 0,
						.format = VK_FORMAT_R32G32B32_SFLOAT,
						.offset = offsetof(Vertex, color),
					},
					VkVertexInputAttributeDescription{
						.location = 3,
						.binding = 0,
						.format = VK_FORMAT_R32G32_SFLOAT,
						.offset = offsetof(Vertex, texCoord),
					},
				},
				.bindings = {
					VkVertexInputBindingDescription{
						.binding = 0,
						.stride = sizeof(Vertex),
						.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
					}
				}
			}
		};

		pipeline = std::make_unique<Vulkan::Pipeline>(device, *pipelineLayout, *renderPass, spec);
	}

	void Renderer::CreateSampler()
	{
		auto properties = device.GetPhysicalDeviceProperties();

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(device.GetHandle(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	void Renderer::CreateFrames()
	{
		VkExtent2D extent = swapchain->GetImageExtent();
		VkFormat format = swapchain->GetImageFormat();

		for (auto image : swapchain->GetImages())
		{
			auto swapchainImage = std::make_unique<Vulkan::Image>(device, image, format);
			auto depthImage = std::make_unique<Vulkan::Image>(device, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D32_SFLOAT, extent.width, extent.height);

			auto target = TargetBuilder()
				.AddImage(std::move(swapchainImage))
				.AddImage(std::move(depthImage))
				.Build(device);

			auto frame = std::make_unique<Frame>(device, std::move(target));

			VkDescriptorSetAllocateInfo allocateInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				.pNext = nullptr,
				.descriptorPool = descriptorPool,
				.descriptorSetCount = 1,
				.pSetLayouts = &descriptorSetLayout,
			};

			vkAllocateDescriptorSets(device.GetHandle(), &allocateInfo, &frame->descriptorSet);

			VkDescriptorBufferInfo bufferInfo = {
				.buffer = frame->uniformBuffer->GetHandle(),
				.offset = 0,
				.range = sizeof(GlobalUniform),
			};

			VkDescriptorImageInfo imageInfo = {
				.sampler = sampler,
				.imageView = texture->GetImageView().GetHandle(),
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			};

			VkWriteDescriptorSet uniformBufferWrite = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = frame->descriptorSet,
				.dstBinding = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pBufferInfo = &bufferInfo,
			};

			VkWriteDescriptorSet samplerWrite = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = frame->descriptorSet,
				.dstBinding = 1,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.pImageInfo = &imageInfo,
			};

			std::array<VkWriteDescriptorSet, 2> writes{ uniformBufferWrite, samplerWrite };

			vkUpdateDescriptorSets(device.GetHandle(), writes.size(), writes.data(), 0, nullptr);

			frames.emplace_back(std::move(frame));
		}
	}

	void Renderer::CreateFramebuffers()
	{
		VkExtent2D extent = swapchain->GetImageExtent();

		for (auto& frame : frames)
		{
			framebuffers.push_back(std::make_unique<Vulkan::Framebuffer>(device, *renderPass, frame->GetTarget().GetViews(), extent));
		}
	}

	void Renderer::Render()
	{
		auto& previousFrame = frames[currentImageIndex];

		Vulkan::Semaphore& acquireSemaphore = previousFrame->RequestSemaphore();

		{
			auto result = swapchain->AcquireNextImageIndex(currentImageIndex, acquireSemaphore);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				bool recreated = RecreateSwapchain(result == VK_ERROR_OUT_OF_DATE_KHR);

				if (recreated)
				{
					result = swapchain->AcquireNextImageIndex(currentImageIndex, acquireSemaphore);
				}

				if (result != VK_SUCCESS)
				{
					previousFrame.reset();
					return;
				}
			}
		}

		frames[currentImageIndex]->Reset();

		Vulkan::Semaphore& renderFinishedSemaphore = frames[currentImageIndex]->RequestSemaphore();
		Vulkan::Fence& renderFence = frames[currentImageIndex]->GetRenderFence();

		Vulkan::CommandBuffer& commandBuffer = frames[currentImageIndex]->RequestCommandBuffer();

		RecordCommandBuffer(commandBuffer);

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore.GetHandle() };
		VkSemaphore waitSemaphores[] = { acquireSemaphore.GetHandle() };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer.GetHandle();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(device.GetGraphicsQueue().GetHandle(), 1, &submitInfo, renderFence.GetHandle()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain->GetHandle();
		presentInfo.pImageIndices = &currentImageIndex;
		presentInfo.pResults = nullptr; // Optional

		{
			auto result = vkQueuePresentKHR(device.GetPresentQueue().GetHandle(), &presentInfo);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				RecreateSwapchain();
			}
			else if (result != VK_SUCCESS)
			{
				throw std::runtime_error("failed to present swap chain image!");
			}
		}
	}

	bool Renderer::RecreateSwapchain(bool force)
	{
		auto details = device.GetSurfaceSupportDetails();

		VkExtent2D currentExtent = details.capabilities.currentExtent;
		VkExtent2D swapchainExtent = swapchain->GetImageExtent();

		if (currentExtent.width == swapchainExtent.width && currentExtent.height == swapchainExtent.height && !force)
		{
			return false;
		}

		device.WaitIdle();

		swapchain->Recreate(currentExtent.width, currentExtent.height);

		VkExtent2D extent = swapchain->GetImageExtent();
		VkFormat format = swapchain->GetImageFormat();

		auto frameIterator = frames.begin();

		for (auto& image : swapchain->GetImages())
		{
			auto swapchainImage = std::make_unique<Vulkan::Image>(device, image, format);
			auto depthImage = std::make_unique<Vulkan::Image>(device, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D32_SFLOAT, extent.width, extent.height);

			auto target = TargetBuilder()
				.AddImage(std::move(swapchainImage))
				.AddImage(std::move(depthImage))
				.Build(device);

			(*frameIterator)->SetTarget(std::move(target));

			frameIterator++;
		}

		framebuffers.clear();

		CreateFramebuffers();

		return true;
	}

	void Renderer::RecordCommandBuffer(Vulkan::CommandBuffer& commandBuffer)
	{
		VkExtent2D extent = swapchain->GetImageExtent();

		commandBuffer.Begin();

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass->GetHandle();
		renderPassInfo.framebuffer = framebuffers[currentImageIndex]->GetHandle();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extent;

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		VkClearValue clearDepth{
			.depthStencil = {
				.depth = 1.f,
			},
		};

		VkClearValue clearValues[] = { clearColor, clearDepth };

		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(commandBuffer.GetHandle(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetHandle());

		VkViewport viewport{};
		viewport.width = static_cast<float>(extent.width);
		viewport.height = static_cast<float>(extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer.GetHandle(), 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		vkCmdSetScissor(commandBuffer.GetHandle(), 0, 1, &scissor);

		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)extent.width / (float)extent.height, 0.1f, 200.0f);
		projection[1][1] *= -1;

		GlobalUniform uniform{
			.viewProjection = projection * view,
			.model = glm::rotate(glm::mat4(1.f), glm::radians(30.f) * 0.f, glm::vec3(0.f, 0.f, 1.f)),
		};

		frames[currentImageIndex]->uniformBuffer->SetData(&uniform, sizeof(GlobalUniform));

		vkCmdBindDescriptorSets(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout->GetHandle(), 0, 1, &frames[currentImageIndex]->descriptorSet, 0, nullptr);

		mesh->Draw(commandBuffer.GetHandle());

		vkCmdEndRenderPass(commandBuffer.GetHandle());

		commandBuffer.End();
	}
}
