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
	protected:
		virtual void OnUpdate(float timestep) {}
		virtual void OnGui() {}
		virtual void OnWindowResize(int width, int height);

		Scene& GetScene();
		void SetScene(Scene& scene);

		Window& GetWindow();

		Vulkan::Device& GetDevice();

	private:
		void SetupVulkan();

		void RenderScene();
		glm::mat4 GetEntityWorldMatrix(Entity entity);

		void SetCameraAspectRatio(Entity entity);

		std::unique_ptr<Window> window;
		std::unique_ptr<Vulkan::Instance> instance;
		std::unique_ptr<Vulkan::Surface> surface;
		std::unique_ptr<Vulkan::PhysicalDevice> physicalDevice;
		std::unique_ptr<Vulkan::Device> device;

		std::unique_ptr<Scene> scene;

		bool running = false;
	};

	std::unique_ptr<Application> CreateApplication(ApplicationArgs args);
}