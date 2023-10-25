#include "Application.hpp"

#include "glm/gtc/random.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "GLFW/glfw3.h"

#include "Vulkan/DescriptorPool.hpp"

#include "Scene/Scene.hpp"
#include "Scene/Components.hpp"

#include "Rendering/Gui.hpp"

#include "Resource/ResourceManager.hpp"
#include "Resource/ResourceRegistry.hpp"

#include "WindowInput.hpp"

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
				if (width == 0 || height == 0)
				{
					return;
				}

				OnWindowResize(width, height);
			}
		);

		SetupVulkan();

		Renderer::Setup(*device, *surface, *window);

		ResourceManager::Create(*device);
		ResourceRegistry::Create();
		Input::Create<WindowInput>(*window);

		scene = std::make_unique<Scene>();
		scene->OnComponentAdded<Component::Camera, &Application::SetCameraAspectRatio>(this);

		renderPipeline = std::make_unique<RenderPipeline>(*device, *scene);

		Gui::Setup(*instance, *device, *physicalDevice, *window, renderPipeline->GetMainRenderPass());

		running = true;
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

		Container::TearDown();
	}

	void Application::Run()
	{
		auto lastTime = std::chrono::high_resolution_clock::now();

		while (running)
		{
			window->Update();
			scene->Update();

			auto currentTime = std::chrono::high_resolution_clock::now();
			auto timestep = std::chrono::duration<float>(currentTime - lastTime);
			lastTime = currentTime;

			if (auto commandBuffer = Renderer::Get().Begin())
			{
				OnUpdate(timestep.count());

				renderPipeline->Draw(*commandBuffer);

				Gui::Get().Begin();

				OnGui();

				Gui::Get().End(*commandBuffer);

				Renderer::Get().End(*commandBuffer);
			}
		}
	}

	void Application::OnWindowResize(int width, int height)
	{
		scene->ForEachEntity<Component::Camera>([&](auto entity) {
			SetCameraAspectRatio(entity);
		});
	}

	void Application::SetCameraAspectRatio(Entity entity)
	{
		auto [height, width] = window->GetFramebufferSize();

		auto& comp = entity.GetComponent<Component::Camera>();

		comp.camera.SetAspectRatio((float)width / height);
	}

	void Application::Exit()
	{
		running = false;
	}

	void Application::SetScene(Scene& scene)
	{
		*this->scene = scene;
	}

	Scene& Application::GetScene()
	{
		return *scene;
	}

	Window& Application::GetWindow()
	{
		return *window;
	}

	Vulkan::Device& Application::GetDevice()
	{
		return *device;
	}
}
