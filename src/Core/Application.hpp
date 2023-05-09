#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <tiny_obj_loader.h>

#include <stdexcept>
#include <vector>
#include <string.h>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <iostream>

#include "Window.hpp"
#include "WindowSurface.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"

#include "Vulkan/Instance.hpp"
#include "Vulkan/PhysicalDevice.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Semaphore.hpp"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 3;

struct RenderFrame
{
	std::unique_ptr<Vulkan::Semaphore> acquireSemaphore;
	std::unique_ptr<Vulkan::Semaphore> renderSemaphore;

	VkFence renderFence;
	VkCommandBuffer commandBuffer;
};


class Application
{
public:
	void run()
	{
		init();
		MainLoop();
		cleanup();
	}

private:
	std::unique_ptr<Window> window;
	std::unique_ptr<Vulkan::Instance> instance;
	std::unique_ptr<WindowSurface> surface;
	Vulkan::PhysicalDevice physicalDevice;
	std::unique_ptr<Vulkan::Device> device;
	std::unique_ptr<Vulkan::Swapchain> swapchain;

	std::unique_ptr<Renderer> renderer;

	std::vector<std::unique_ptr<RenderFrame>> frames;

	uint32_t currentFrame = 0;
	bool windowHasResized = false;

	void init()
	{
		window = WindowBuilder()
			.Height(HEIGHT)
			.Width(WIDTH)
			.UserPointer(this)
			.Build();

		window->SetFramebufferSizeCallback(
			[](Window* window, FramebufferSize size)
			{
				auto app = window->GetUserPointer<Application>();
				app->windowHasResized = true;
			}
		);

		instance = Vulkan::InstanceBuilder().Build();

		surface = std::make_unique<WindowSurface>(*instance, window->GetHandle());

		physicalDevice = Vulkan::PhysicalDevicePicker::PickBestSuitable(*instance, surface->GetHandle());

		device = std::make_unique<Vulkan::Device>(*instance, physicalDevice);

		auto size = window->GetFramebufferSize();

		swapchain = Vulkan::SwapchainBuilder()
			.DesiredWidth(size.width)
			.DesiredHeight(size.height)
			.MaxFramesInFlight(MAX_FRAMES_IN_FLIGHT)
			.Build(*device, *surface);

		renderer = std::make_unique<Renderer>(*device, *swapchain);

		CreateFrames();
	}

	void CreateFrames()
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = device->GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			std::unique_ptr<RenderFrame> frame = std::make_unique<RenderFrame>();
			if (vkCreateFence(device->GetHandle(), &fenceInfo, nullptr, &frame->renderFence) != VK_SUCCESS)
			{

				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}

			if (vkAllocateCommandBuffers(device->GetHandle(), &allocInfo, &frame->commandBuffer) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate command buffers!");
			}

			frame->acquireSemaphore = std::make_unique<Vulkan::Semaphore>(*device);
			frame->renderSemaphore = std::make_unique<Vulkan::Semaphore>(*device);

			frames.emplace_back(std::move(frame));
		}
	}

	void MainLoop()
	{
		while (!window->ShouldClose())
		{
			window->Update();
			Render();
		}

		device->WaitIdle();
	}

	void Render()
	{
		vkWaitForFences(device->GetHandle(), 1, &frames[currentFrame]->renderFence, VK_TRUE, UINT64_MAX);

		uint32_t imageIndex = swapchain->GetNextImageIndex(*frames[currentFrame]->acquireSemaphore);

		vkResetFences(device->GetHandle(), 1, &frames[currentFrame]->renderFence);
		vkResetCommandBuffer(frames[currentFrame]->commandBuffer, 0);

		renderer->Render(frames[currentFrame]->commandBuffer, imageIndex);

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[] = { frames[currentFrame]->renderSemaphore->GetHandle() };
		VkSemaphore waitSemaphores[] = { frames[currentFrame]->acquireSemaphore->GetHandle() };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &frames[currentFrame]->commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, frames[currentFrame]->renderFence) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapchains[] = { swapchain->GetHandle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;

		presentInfo.pResults = nullptr; // Optional

		auto result = vkQueuePresentKHR(device->GetPresentQueue(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowHasResized)
		{
			windowHasResized = false;
			RecreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void RecreateSwapchain()
	{
		window->WaitForFocus();
		device->WaitIdle();
		
		renderer->ResetImages();

		auto size = window->GetFramebufferSize();
		swapchain->Recreate(size.width, size.height);

		renderer->CreateImages();
	}

	void cleanup()
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyFence(device->GetHandle(), frames[i]->renderFence, nullptr);
		}
	}
};