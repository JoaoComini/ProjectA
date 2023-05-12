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
#include "Rendering/Renderer.hpp"

#include "Window.hpp"
#include "WindowSurface.hpp"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

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