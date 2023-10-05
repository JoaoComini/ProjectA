#include "Application.hpp"

#include "glm/gtc/random.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "GLFW/glfw3.h"

#include "Vulkan/DescriptorPool.hpp"

#include "Scene/Scene.hpp"
#include "Scene/Components.hpp"

#include "Rendering/Gui.hpp"

#include "Resource/ResourceManager.hpp"
#include "Resource/Model.hpp"

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

		SetupVulkan();

		renderer = Renderer::Setup(*device, *surface, *window);
		gui = Gui::Setup(*instance, *device, *physicalDevice, *window);

		ResourceManager::Setup(*device);

		Input::Setup(*window);

		scene = std::make_unique<Scene>();

		AddSystem<RenderSystem>();
	}

	void Application::SetupVulkan()
	{
		instance = Vulkan::InstanceBuilder().Build();

		surface = window->CreateSurface(*instance);

		physicalDevice = Vulkan::PhysicalDevicePicker::PickBestSuitable(*instance, *surface);

		device = std::make_unique<Vulkan::Device>(*instance, *physicalDevice);
	}

	Application::~Application()
	{
		device->WaitIdle();

		ResourceManager::DeleteInstance();
		Input::DeleteInstance();
		Gui::DeleteInstance();
		Renderer::DeleteInstance();
	}

	void Application::Run()
	{
		running = true;

		auto lastTime = std::chrono::high_resolution_clock::now();

		while (running)
		{
			window->Update();
			scene->Update();

			auto currentTime = std::chrono::high_resolution_clock::now();
			auto timestep = std::chrono::duration<float>(currentTime - lastTime);
			lastTime = currentTime;

			auto [camera, found] = scene->FindFirstEntity<Component::Transform, Component::Camera>();

			if (!found)
			{
				return;
			}

			Component::Camera cameraComponent = camera.GetComponent<Component::Camera>();
			Component::Transform cameraTransform = camera.GetComponent<Component::Transform>();

			renderer->Begin(cameraComponent.camera, cameraTransform.GetLocalMatrix());
			gui->Begin();

			for (auto& system : systems)
			{
				system->Update(timestep.count());
			}

			OnGui();

			gui->End(renderer->GetActiveCommandBuffer());
			renderer->End();
		}
	}

	void Application::Exit()
	{
		running = false;
	}

	Scene& Application::GetScene()
	{
		return *scene;
	}

	Window& Application::GetWindow()
	{
		return *window;
	}
}
