#include "Application.hpp"

#include "glm/gtc/random.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "Rendering/Model.hpp"

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
	//Rendering::Texture diffuse = Rendering::Texture(*device, "resources/models/viking_room.png");
	//Rendering::Material material = Rendering::Material(&diffuse);

	//Rendering::Mesh mesh = Rendering::Mesh(*device, material, "resources/models/viking_room.obj");


	Rendering::Model model(*device, "resources/models/duck.glb");

	auto size = window->GetFramebufferSize();

	Rendering::Camera camera = Rendering::Camera(glm::radians(45.f), (float)size.width / (float)size.height, 0.1f, 2000.0f);

	window->OnResize(
		[&camera](int width, int height) {
			camera.SetAspect((float)width / height);
		}
	);

	auto transform = glm::translate(glm::mat4(1.f), glm::vec3(-400));

	while (!window->ShouldClose())
	{
		window->Update();

		renderer->Begin(camera);

		for (auto& mesh : model.GetMeshes())
		{
			renderer->Draw(*mesh, transform);
		}

		renderer->End();
	}

	device->WaitIdle();
}