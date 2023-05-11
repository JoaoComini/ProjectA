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

#include "Vulkan/Instance.hpp"
#include "Vulkan/PhysicalDevice.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Semaphore.hpp"
#include "Vulkan/Semaphore.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Frame.hpp"

#include "Window.hpp"
#include "WindowSurface.hpp"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

class Application
{
public:
	void Run()
	{
		Init();
		MainLoop();
	}

private:
	std::unique_ptr<Window> window;
	std::unique_ptr<Vulkan::Instance> instance;
	std::unique_ptr<WindowSurface> surface;
	Vulkan::PhysicalDevice physicalDevice;
	std::unique_ptr<Vulkan::Device> device;
	std::unique_ptr<Vulkan::Swapchain> swapchain;

	std::unique_ptr<Renderer> renderer;

	std::vector<std::unique_ptr<Rendering::Frame>> frames;

	uint32_t currentFrame = 0;
	bool windowHasResized = false;

	void Init()
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
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			frames.emplace_back(std::make_unique<Rendering::Frame>(*device));
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
		Vulkan::Fence& renderFence = frames[currentFrame]->GetRenderFence();
		Vulkan::Semaphore& acquireSemaphore = frames[currentFrame]->GetAcquireSemaphore();
		Vulkan::Semaphore& renderFinishedSemaphore = frames[currentFrame]->GetRenderFinishedSemaphore();

		uint32_t imageIndex = swapchain->GetNextImageIndex(acquireSemaphore);

		frames[currentFrame]->Reset();

		Vulkan::CommandBuffer& commandBuffer = frames[currentFrame]->RequestCommandBuffer();

		renderer->Render(commandBuffer, imageIndex);

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore.GetHandle() };
		VkSemaphore waitSemaphores[] = { acquireSemaphore.GetHandle() };

		VkCommandBuffer commandBuffers[] = { commandBuffer.GetHandle() };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, renderFence.GetHandle()) != VK_SUCCESS)
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
};