#include "Renderer.hpp"

#include <array>

#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/Shader.hpp"

#include "Vertex.hpp"

namespace Engine
{

	Renderer::Renderer(Vulkan::Device& device, const Vulkan::Surface& surface, const Window& window)
		: device(device), surface(surface)
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
		{
			VkDescriptorSetLayoutBinding uniformBufferBinding = {
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
			};

			globalDescriptorSetLayout = std::make_shared<Vulkan::DescriptorSetLayout>(
				device,
				std::vector<VkDescriptorSetLayoutBinding>{ uniformBufferBinding }
			);
		}

		{
			VkDescriptorSetLayoutBinding uniformBufferBinding = {
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
			};

			VkDescriptorSetLayoutBinding samplerBinding = {
				.binding = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
				.pImmutableSamplers = nullptr,
			};

			modelDescriptorSetLayout = std::make_shared<Vulkan::DescriptorSetLayout>(
				device,
				std::vector<VkDescriptorSetLayoutBinding>{ uniformBufferBinding, samplerBinding }
			);
		}

	}

	void Renderer::CreatePipeline()
	{
		pipelineLayout = std::make_unique<Vulkan::PipelineLayout>(
			device,
			std::vector<std::shared_ptr<Vulkan::DescriptorSetLayout>>{ globalDescriptorSetLayout, modelDescriptorSetLayout }
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
			auto target = CreateTarget(image, format, extent);

			auto frame = std::make_unique<Frame>(device, std::move(target));

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

	void Renderer::Begin(const Camera& camera, const glm::mat4& transform)
	{
		globalUniform.viewProjection = camera.GetProjection() * glm::inverse(transform);

		auto& previousFrame = GetCurrentFrame();

		acquireSemaphore = &previousFrame.RequestSemaphore();

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
				previousFrame.Reset();
				return;
			}
		}

		auto& frame = GetCurrentFrame();
		frame.Reset();

		activeCommandBuffer = &frame.RequestCommandBuffer();

		BeginCommandBuffer();
		UpdateGlobalUniform();
	}

	void Renderer::BeginCommandBuffer()
	{
		activeCommandBuffer->Begin();

		VkExtent2D extent = swapchain->GetImageExtent();

		std::vector<VkClearValue> clearValues = {
			{ { 0.f, 0.f, 0.f, 1.f } },
			{ 1.f, 0.f }
		};

		activeCommandBuffer->BeginRenderPass(*renderPass, *framebuffers[currentImageIndex], clearValues, extent);

		activeCommandBuffer->SetViewport({
			{
				.width = static_cast<float>(extent.width),
				.height = static_cast<float>(extent.height),
				.minDepth = 0.0f,
				.maxDepth = 1.0f,
			}
			});

		activeCommandBuffer->SetScissor({
			{
				.extent = extent
			}
			});

		activeCommandBuffer->BindPipeline(*pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS);
	}

	void Renderer::UpdateGlobalUniform()
	{
		auto& frame = GetCurrentFrame();

		auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(GlobalUniform));

		allocation.SetData(&globalUniform);

		BindingMap<VkDescriptorBufferInfo> bufferInfos = {
			{ 0,{ { 0, VkDescriptorBufferInfo{
				.buffer = allocation.GetBuffer().GetHandle(),
				.offset = allocation.GetOffset(),
				.range = allocation.GetSize(),
		} } } }
		};

		auto descriptorSet = frame.RequestDescriptorSet(*globalDescriptorSetLayout, bufferInfos, {});

		activeCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout, 0, descriptorSet);
	}


	void Renderer::Draw(const Mesh& mesh, const glm::mat4& transform)
	{
		const Material* material = mesh.GetMaterial();

		ModelUniform uniform{
			.model = transform,
			.color = material->GetColor(),
		};

		auto& frame = GetCurrentFrame();

		auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(ModelUniform));

		allocation.SetData(&uniform);

		BindingMap<VkDescriptorBufferInfo> bufferInfos = {
			{ 0, { { 0, VkDescriptorBufferInfo{
				.buffer = allocation.GetBuffer().GetHandle(),
				.offset = allocation.GetOffset(),
				.range = allocation.GetSize(),
			} } } }
		};

		BindingMap<VkDescriptorImageInfo> imageInfos = {
			{ 1, { { 0, VkDescriptorImageInfo{
				.sampler = material->GetDiffuse()->GetSampler().GetHandle(),
				.imageView = material->GetDiffuse()->GetImageView().GetHandle(),
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			} } } }
		};

		auto descriptorSet = frame.RequestDescriptorSet(*modelDescriptorSetLayout, bufferInfos, imageInfos);

		activeCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout, 1, descriptorSet);

		mesh.Draw(activeCommandBuffer->GetHandle());
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
		auto& frame = GetCurrentFrame();
		Vulkan::Semaphore& renderFinishedSemaphore = frame.RequestSemaphore();
		Vulkan::Fence& renderFence = frame.GetRenderFence();

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
			auto target = CreateTarget(image, format, extent);

			(*it)->SetTarget(std::move(target));

			it++;
		}

		framebuffers.clear();

		CreateFramebuffers();

		return true;
	}

	Frame& Renderer::GetCurrentFrame() const
	{
		return *frames[currentImageIndex];
	}

	std::unique_ptr<Target> Renderer::CreateTarget(VkImage image, VkFormat format, VkExtent2D extent)
	{
		auto swapchainImage = std::make_unique<Vulkan::Image>(device, image, format);

		auto depthImage = std::make_unique<Vulkan::Image>(
			device,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_FORMAT_D32_SFLOAT,
			VkExtent3D{ extent.width, extent.height, 1 },
			device.GetMaxSampleCount()
		);

		auto colorImage = std::make_unique<Vulkan::Image>(
			device,
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			format,
			VkExtent3D{ extent.width, extent.height, 1 },
			device.GetMaxSampleCount()
		);

		return TargetBuilder()
			.AddImage(std::move(colorImage))
			.AddImage(std::move(depthImage))
			.AddImage(std::move(swapchainImage))
			.Build(device);
	}
}
