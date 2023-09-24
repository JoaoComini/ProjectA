#pragma once

#include <memory>
#include <vector>

#include "Vulkan/Instance.hpp"
#include "Vulkan/PhysicalDevice.hpp"
#include "Vulkan/Device.hpp"

#include "Rendering/Renderer.hpp"
#include "Scene/EntityManager.hpp"

#include "Window.hpp"
#include "WindowSurface.hpp"
#include "System.hpp"


class Application
{
public:
	Application();
	void Run();

	template<typename T, typename... Args>
	void AddSystem(Args&&... args)
	{
		systems.push_back(std::make_unique<T>(*entityManager, std::forward<Args>(args)...));
	}


private:
	void CreateRenderer();
	void CreateCamera();

	std::unique_ptr<Window> window;
	std::unique_ptr<Vulkan::Instance> instance;
	std::unique_ptr<WindowSurface> surface;
	std::unique_ptr<Vulkan::PhysicalDevice> physicalDevice;
	std::unique_ptr<Vulkan::Device> device;

	std::vector<std::unique_ptr<System>> systems;

	std::unique_ptr<Rendering::Renderer> renderer;

	std::unique_ptr<Scene::EntityManager> entityManager;


};