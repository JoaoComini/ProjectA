#include "Application.hpp"

#include "glm/gtc/random.hpp"

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
	Rendering::Texture diffuse = Rendering::Texture(*device, "resources/models/viking_room.png");
	Rendering::Material material = Rendering::Material(&diffuse);

	auto size = window->GetFramebufferSize();

	Rendering::Camera camera = Rendering::Camera(glm::radians(45.f), (float)size.width / (float)size.height, 0.1f, 200.0f);

	window->OnResize(
		[&camera](int width, int height) {
			camera.SetAspect((float)width / height);
		}
	);

	std::vector<glm::mat4> transforms(1000);

	std::generate(transforms.begin(), transforms.end(), []() {
		glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::sphericalRand(10.f));
		return glm::rotate(transform, glm::radians(45.f), glm::vec3(0.f, 0.f, 1.f));
	});


	while (!window->ShouldClose())
	{
		window->Update();

		renderer->Begin(camera);
		for (int i = 0; i < transforms.size(); i++)
		{
			renderer->Draw(mesh, material, transforms[i]);
		}
		renderer->End();
	}

	device->WaitIdle();
}