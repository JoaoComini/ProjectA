#include "Renderer.hpp"

#include <array>

#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/Shader.hpp"

#include "Vertex.hpp"

namespace Rendering
{

	Renderer::Renderer(Vulkan::Device& device, const Vulkan::Surface& surface, const Window& window)
		: device(device), surface(surface), camera(camera)
	{
		auto size = window.GetFramebufferSize();

		swapchain = Vulkan::SwapchainBuilder()
			.DesiredWidth(size.width)
			.DesiredHeight(size.height)
			.MinImageCount(3)
			.Build(device, surface);

		renderPass = std::make_unique<Vulkan::RenderPass>(device, *swapchain);

		CreateDescriptors();
		CreatePipeline();
		CreateFrames();
		CreateFramebuffers();
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

		descriptorSetLayout = std::make_shared<Vulkan::DescriptorSetLayout>(
			device,
			std::vector<VkDescriptorSetLayoutBinding>{ uniformBufferBinding, samplerBinding }
		);

		descriptorPool = std::make_unique<Vulkan::DescriptorPool>(device, *descriptorSetLayout, 10);
	}

	void Renderer::CreatePipeline()
	{
		pipelineLayout = std::make_unique<Vulkan::PipelineLayout>(
			device,
			std::vector<std::shared_ptr<Vulkan::DescriptorSetLayout>>{ descriptorSetLayout }
		);

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
						.format = VK_FORMAT_R32G32_SFLOAT,
						.offset = offsetof(Vertex, uv),
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

	void Renderer::CreateFrames()
	{
		VkExtent2D extent = swapchain->GetImageExtent();
		VkFormat format = swapchain->GetImageFormat();

		for (auto image : swapchain->GetImages())
		{
			auto swapchainImage = std::make_unique<Vulkan::Image>(device, image, format);
			auto depthImage = std::make_unique<Vulkan::Image>(device, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D32_SFLOAT, VkExtent3D{ extent.width, extent.height, 1 });

			auto target = TargetBuilder()
				.AddImage(std::move(swapchainImage))
				.AddImage(std::move(depthImage))
				.Build(device);

			auto frame = std::make_unique<Frame>(device, *descriptorSetLayout, std::move(target));

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

	void Renderer::Begin(Camera& camera)
	{
		this->camera = &camera;

		auto& previousFrame = frames[currentImageIndex];

		acquireSemaphore = &previousFrame->RequestSemaphore();

		auto result = swapchain->AcquireNextImageIndex(currentImageIndex, *acquireSemaphore);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			bool recreated = RecreateSwapchain(result == VK_ERROR_OUT_OF_DATE_KHR);

			if (recreated)
			{
				result = swapchain->AcquireNextImageIndex(currentImageIndex, *acquireSemaphore);
			}

			if (result != VK_SUCCESS)
			{
				previousFrame.reset();
				return;
			}
		}

		frames[currentImageIndex]->Reset();

		activeCommandBuffer = &frames[currentImageIndex]->RequestCommandBuffer();

		BeginCommandBuffer();
	}

	void Renderer::Draw(Mesh& mesh, Material& material, glm::mat4 transform)
	{
		GlobalUniform uniform {
			.viewProjection = camera->GetViewProjection(),
			.model = transform,
		};

		auto& frame = frames[currentImageIndex];

		auto& buffer = frame->RequestBuffer(Vulkan::BufferUsageFlags::UNIFORM, sizeof(GlobalUniform));

		buffer.SetData(&uniform, sizeof(GlobalUniform));

		BindingMap<VkDescriptorBufferInfo> bufferInfos = {
			{ 0, { { 0, VkDescriptorBufferInfo{
				.buffer = buffer.GetHandle(),
				.offset = 0,
				.range = sizeof(GlobalUniform),
			} } } }
		};

		BindingMap<VkDescriptorImageInfo> imageInfos = {
			{ 1, { { 0, VkDescriptorImageInfo{
				.sampler = material.GetDiffuse()->GetSampler().GetHandle(),
				.imageView = material.GetDiffuse()->GetImageView().GetHandle(),
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			} } } }
		};

		auto descriptorSet = frame->RequestDescriptorSet(bufferInfos, imageInfos);

		activeCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout, descriptorSet);

		mesh.Draw(activeCommandBuffer->GetHandle());
	}

	void Renderer::BeginCommandBuffer()
	{
		activeCommandBuffer->Begin();

		VkExtent2D extent = swapchain->GetImageExtent();

		std::vector<VkClearValue> clearValues = {
			{ { 0.0f, 0.0f, 0.0f, 1.0f } },
			{ 1.0f, 0 }
		};

		activeCommandBuffer->BeginRenderPass(*renderPass, *framebuffers[currentImageIndex], clearValues, extent);

		std::vector<VkViewport> viewports = {
			{
				.width = static_cast<float>(extent.width),
				.height = static_cast<float>(extent.height),
				.minDepth = 0.0f,
				.maxDepth = 1.0f,
			}
		};

		activeCommandBuffer->SetViewport(viewports);

		std::vector<VkRect2D> scissors = {
			{
				.extent = extent
			}
		};

		activeCommandBuffer->SetScissor(scissors);

		activeCommandBuffer->BindPipeline(*pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS);
	}

	void Renderer::End()
	{
		activeCommandBuffer->EndRenderPass();
		activeCommandBuffer->End();

		Vulkan::Semaphore& waitSemaphore = Submit();

		Present(waitSemaphore);

		activeCommandBuffer = nullptr;
		acquireSemaphore = nullptr;
	}

	Vulkan::Semaphore& Renderer::Submit()
	{
		Vulkan::Semaphore& renderFinishedSemaphore = frames[currentImageIndex]->RequestSemaphore();
		Vulkan::Fence& renderFence = frames[currentImageIndex]->GetRenderFence();

		device.GetGraphicsQueue().Submit(*activeCommandBuffer, *acquireSemaphore, renderFinishedSemaphore, renderFence);

		return renderFinishedSemaphore;
	}

	void Renderer::Present(Vulkan::Semaphore& waitSemaphore)
	{
		auto result = device.GetPresentQueue().Present(*swapchain, waitSemaphore, currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			RecreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
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

		auto it = frames.begin();

		for (auto& image : swapchain->GetImages())
		{
			auto swapchainImage = std::make_unique<Vulkan::Image>(device, image, format);
			auto depthImage = std::make_unique<Vulkan::Image>(device, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D32_SFLOAT, VkExtent3D{ extent.width, extent.height, 1 });

			auto target = TargetBuilder()
				.AddImage(std::move(swapchainImage))
				.AddImage(std::move(depthImage))
				.Build(device);

			(*it)->SetTarget(std::move(target));

			it++;
		}

		framebuffers.clear();

		CreateFramebuffers();

		return true;
	}
}
