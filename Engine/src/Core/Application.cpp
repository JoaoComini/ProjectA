#include "Application.hpp"

#include "glm/gtc/random.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "GLFW/glfw3.h"

#include "Scene/Scene.hpp"
#include "Scene/Components.hpp"

#include "Rendering/Gui.hpp"
#include "Rendering/Renderer.hpp"

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

		scene = std::make_unique<Scene>();
		scene->OnComponentAdded<Component::Camera, &Application::SetCameraAspectRatio>(this);

		scriptRunner = std::make_unique<ScriptRunner>(*scene);

		Renderer::Create(*window, *scene);
		Gui::Create(*window);
		ResourceManager::Create();
		ResourceRegistry::Create();
		Input::Create<WindowInput>(*window);

		running = true;
	}

	Application::~Application()
	{
		Container::TearDown();
	}

	void Application::Run()
	{
		auto lastTime = std::chrono::high_resolution_clock::now();

		while (running)
		{
			window->Update();
			scene->Cleanup();

			auto currentTime = std::chrono::high_resolution_clock::now();
			auto timestep = std::chrono::duration<float>(currentTime - lastTime);
			lastTime = currentTime;

			OnUpdate(timestep.count());

			auto& commandBuffer = Renderer::Get().Begin();

			Renderer::Get().Draw();

			Gui::Get().Begin();

			OnGui();

			Gui::Get().End(commandBuffer);

			Renderer::Get().End();
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

		auto& camera = comp.camera;

		camera.SetAspectRatio((float)width / height);
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


	void Application::StartScripts()
	{
		scriptRunner->Start();
	}


	void Application::UpdateScripts(float timestep)
	{
		scriptRunner->Update(timestep);
	}

	void Application::StopScripts()
	{
		scriptRunner->Stop();
	}
}
