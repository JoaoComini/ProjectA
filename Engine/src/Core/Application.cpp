#include "Application.hpp"

#include "glm/gtc/random.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "Scene/Loader.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Components.hpp"

Application::Application()
{
	window = WindowBuilder()
		.Width(800)
		.Height(600)
		.Build();

	instance = Vulkan::InstanceBuilder().Build();

	surface = std::make_unique<WindowSurface>(*instance, window->GetHandle());

	physicalDevice = Vulkan::PhysicalDevicePicker::PickBestSuitable(*instance, *surface);

	device = std::make_unique<Vulkan::Device>(*instance, *physicalDevice);

	renderer = std::make_unique<Rendering::Renderer>(*device, *surface, *window);
}

void Application::Run()
{

	auto size = window->GetFramebufferSize();

	Rendering::Camera camera(glm::radians(45.f), (float)size.width / (float)size.height, 0.1f, 2000.0f);

	window->OnResize(
		[&camera](int width, int height) {
			camera.SetAspect((float)width / height);
		}
	);

	Scene::Scene scene;
	Scene::Loader loader(*device, scene);

	loader.LoadFromGltf("resources/models/Lantern.glb");

	while (!window->ShouldClose())
	{
		window->Update();

		renderer->Begin(camera);

		scene.Render(*renderer);

		renderer->End();
	}

	device->WaitIdle();
}