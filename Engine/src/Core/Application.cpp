#include "Application.hpp"

#include "glm/gtc/random.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "Scene/Loader.hpp"
#include "Scene/EntityManager.hpp"
#include "Scene/Components.hpp"

#include "RenderSystem.hpp"

#include <iostream>

Application::Application()
{
	window = WindowBuilder()
		.Width(800)
		.Height(600)
		.Build();

	entityManager = std::make_unique<Scene::EntityManager>();
}

void Application::Run()
{
	CreateRenderer();
	CreateCamera();

	Scene::Loader loader(*device, *entityManager);
	loader.LoadFromGltf("resources/models/Lantern.glb");

	auto lastTime = std::chrono::high_resolution_clock::now();

	AddSystem<RenderSystem>(*renderer);

	while (!window->ShouldClose())
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

	device->WaitIdle();
}

void Application::CreateRenderer()
{
	instance = Vulkan::InstanceBuilder().Build();

	surface = std::make_unique<WindowSurface>(*instance, window->GetHandle());

	physicalDevice = Vulkan::PhysicalDevicePicker::PickBestSuitable(*instance, *surface);

	device = std::make_unique<Vulkan::Device>(*instance, *physicalDevice);

	renderer = std::make_unique<Rendering::Renderer>(*device, *surface, *window);
}

void Application::CreateCamera()
{
	auto size = window->GetFramebufferSize();

	auto entity = entityManager->CreateEntity();

	auto& transform = entity.AddComponent<Scene::Component::Transform>();

	transform.position = glm::vec3(0, 10, 50);

	auto camera = Rendering::Camera(glm::radians(45.f), (float)size.width / (float)size.height, 0.1f, 2000.0f);
	auto& component = entity.AddComponent<Scene::Component::Camera>(camera);

	window->OnResize(
		[&](int width, int height) {
			component.camera.SetAspect((float)width / height);
		}
	);
}
