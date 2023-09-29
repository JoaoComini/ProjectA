#include "Application.hpp"

#include "glm/gtc/random.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "GLFW/glfw3.h"

#include "Scene/SceneLoader.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Components.hpp"

#include "RenderSystem.hpp"
#include "Input.hpp"

#include <iostream>

namespace Engine {

	Application::Application(ApplicationSpec &spec)
	{
		window = WindowBuilder()
			.Width(1600)
			.Height(900)
			.Title(spec.name)
			.Build();

		window->OnClose([&]() {
			running = false;
		});

		Input::Setup(*window);

		scene = std::make_unique<Scene>();

		CreateRenderer();
		CreateCamera();

		AddSystem<RenderSystem>(*renderer);
	}

	Application::~Application()
	{
		device->WaitIdle();
	}

	void Application::Run()
	{
		running = true;

		{
			SceneLoader loader(*device, *scene);
			loader.LoadFromGltf("resources/models/Lantern.glb");
		}

		auto lastTime = std::chrono::high_resolution_clock::now();

		while (running)
		{
			window->Update();

			auto currentTime = std::chrono::high_resolution_clock::now();
			auto timestep = std::chrono::duration<float>(currentTime - lastTime);
			lastTime = currentTime;

			for (auto& system : systems)
			{
				system->Update(timestep.count());
			}
		}
	}

	FramebufferSize Application::GetFramebufferSize() const
	{
		return window->GetFramebufferSize();
	}

	void Application::CreateRenderer()
	{
		instance = Vulkan::InstanceBuilder().Build();

		surface = window->CreateSurface(*instance);

		physicalDevice = Vulkan::PhysicalDevicePicker::PickBestSuitable(*instance, *surface);

		device = std::make_unique<Vulkan::Device>(*instance, *physicalDevice);

		renderer = std::make_unique<Renderer>(*device, *surface, *window);
	}

	void Application::CreateCamera()
	{
		window->OnResize(
			[&](int width, int height) {
				auto [entity, found] = scene->FindFirstEntity<Component::Camera, Component::Transform>();

				if (!found)
				{
					return;
				}

				auto& component = entity.GetComponent<Component::Camera>();

				component.camera.SetAspectRatio((float)width / height);
			}
		);
	}
}
