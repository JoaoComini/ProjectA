#include "Application.hpp"

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
	Rendering::Mesh mesh = Rendering::Mesh(*device, "resources/models/viking_room.obj");

	auto size = window->GetFramebufferSize();
	Rendering::Camera camera = Rendering::Camera(glm::radians(45.f), (float)size.width / (float)size.height, 0.1f, 200.0f);

	while (!window->ShouldClose())
	{
		window->Update();

		renderer->Begin(camera);
		renderer->Draw(mesh, glm::rotate(glm::mat4(1.f), glm::radians(45.f), glm::vec3(0.f, 0.f, 1.f)));
		renderer->End();
	}

	device->WaitIdle();
}