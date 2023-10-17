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

	struct ApplicationArgs
	{
		int argc = 0;
		std::vector<std::string> argv;

		const std::string operator[](int index) const
		{
			if (index >= argc)
			{
				return {};
			}

			return argv[index];
		}
	};

	struct ApplicationSpec
	{
		std::string name = "Engine Application";
		ApplicationArgs args{};
	};

	class Application
	{
	public:
		Application(ApplicationSpec &spec);
		virtual ~Application();

		void Run();
		void Exit();
		virtual void OnGui() {}

		Scene& GetScene();

		Window& GetWindow();

		Vulkan::Device& GetDevice();

		template<typename T, typename... Args>
		void AddSystem(Args&&... args)
		{
			systems.push_back(std::make_unique<T>(*scene, std::forward<Args>(args)...));
		}

	private:
		void SetupVulkan();

		void SetCameraAspectRatio(Entity entity);

		std::unique_ptr<Window> window;
		std::unique_ptr<Vulkan::Instance> instance;
		std::unique_ptr<Vulkan::Surface> surface;
		std::unique_ptr<Vulkan::PhysicalDevice> physicalDevice;
		std::unique_ptr<Vulkan::Device> device;

		std::unique_ptr<Scene> scene;

		std::vector<std::unique_ptr<System>> systems;

		bool running = false;
	};

	std::unique_ptr<Application> CreateApplication(ApplicationArgs args);
}