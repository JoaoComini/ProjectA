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
				OnWindowResize(width, height);
			}
		);

		SetupVulkan();

		Renderer::Setup(*device, *surface, *window);
		Gui::Setup(*instance, *device, *physicalDevice, *window);

		ResourceManager::Create(*device);
		ResourceRegistry::Create();
		Input::Create<WindowInput>(*window);

		scene = std::make_unique<Scene>();
		scene->OnComponentAdded<Component::Camera, &Application::SetCameraAspectRatio>(this);

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
				Gui::Get().Begin();

				OnUpdate(timestep.count());

				RenderScene();

				OnGui();

				Gui::Get().End(*commandBuffer);
				Renderer::Get().End();
			}
		}
	}

	void Application::RenderScene()
	{
		scene->ForEachEntity<Component::Transform, Component::MeshRender>(
			[&](Entity entity) {
				auto& meshRender = entity.GetComponent<Component::MeshRender>();

				if (!meshRender.mesh)
				{
					return;
				}

				auto mesh = ResourceManager::Get().LoadResource<Mesh>(meshRender.mesh);

				glm::mat4 matrix = GetEntityWorldMatrix(entity);

				Renderer::Get().Draw(*mesh, matrix);
			}
		);
	}

	glm::mat4 Application::GetEntityWorldMatrix(Entity entity)
	{
		auto parent = entity.GetParent();
		auto& transform = entity.GetComponent<Component::Transform>();

		if (!parent)
		{
			return transform.GetLocalMatrix();
		}

		auto parentTransform = parent.TryGetComponent<Component::Transform>();

		if (!parentTransform)
		{
			return transform.GetLocalMatrix();
		}

		return GetEntityWorldMatrix(parent) * transform.GetLocalMatrix();
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
