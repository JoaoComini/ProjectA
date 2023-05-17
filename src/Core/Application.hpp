#pragma once

#include <memory>

#include "Vulkan/Instance.hpp"
#include "Vulkan/PhysicalDevice.hpp"
#include "Vulkan/Device.hpp"

#include "Window.hpp"
#include "WindowSurface.hpp"

#include "Rendering/Renderer.hpp"

class Application
{
public:
	Application();
	void Run();

private:
	std::unique_ptr<Window> window;
	std::unique_ptr<Vulkan::Instance> instance;
	std::unique_ptr<WindowSurface> surface;
	std::unique_ptr<Vulkan::PhysicalDevice> physicalDevice;
	std::unique_ptr<Vulkan::Device> device;

	std::unique_ptr<Rendering::Renderer> renderer;
};