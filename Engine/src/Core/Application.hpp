#pragma once

#include <memory>
#include <vector>

#include "Vulkan/Instance.hpp"
#include "Vulkan/PhysicalDevice.hpp"
#include "Vulkan/Device.hpp"

#include "Rendering/Renderer.hpp"
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

		template<typename T, typename... Args>
		void AddSystem(Args&&... args)
		{
			systems.push_back(std::make_unique<T>(*scene, std::forward<Args>(args)...));
		}

		FramebufferSize GetFramebufferSize() const;

	private:
		void CreateRenderer();
		void CreateCamera();

		std::unique_ptr<Window> window;
		std::unique_ptr<Vulkan::Instance> instance;
		std::unique_ptr<Vulkan::Surface> surface;
		std::unique_ptr<Vulkan::PhysicalDevice> physicalDevice;
		std::unique_ptr<Vulkan::Device> device;

		std::vector<std::unique_ptr<System>> systems;

		std::unique_ptr<Renderer> renderer;

		std::unique_ptr<Scene> scene;

		bool running = false;
	};

	std::unique_ptr<Application> CreateApplication();
}