#pragma once

#include <memory>
#include <vector>

#include "Vulkan/Instance.hpp"
#include "Vulkan/PhysicalDevice.hpp"
#include "Vulkan/Device.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Gui.hpp"
#include "Scene/Scene.hpp"

#include "Window.hpp"
#include "WindowSurface.hpp"
#include "System.hpp"

namespace Engine {

	struct ApplicationSpec
	{
		std::string name = "Engine Application";
	};

	class Application
	{
	public:
		Application(ApplicationSpec &spec);
		~Application();

		void Run();
		void Exit();
		virtual void OnGui() {}

		Scene& GetScene();
		Window& GetWindow();

		template<typename T, typename... Args>
		void AddSystem(Args&&... args)
		{
			systems.push_back(std::make_unique<T>(*scene, std::forward<Args>(args)...));
		}

	private:
		void SetupVulkan();

		std::unique_ptr<Window> window;
		std::unique_ptr<Vulkan::Instance> instance;
		std::unique_ptr<Vulkan::Surface> surface;
		std::unique_ptr<Vulkan::PhysicalDevice> physicalDevice;
		std::unique_ptr<Vulkan::Device> device;

		Renderer* renderer;
		Gui* gui;

		std::unique_ptr<Scene> scene;

		std::vector<std::unique_ptr<System>> systems;

		bool running = false;
	};

	std::unique_ptr<Application> CreateApplication();
}