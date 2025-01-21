#include "Application.hpp"

#include "glm/gtc/random.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "GLFW/glfw3.h"

#include "Scene/Scene.hpp"
#include "Scene/Components.hpp"

#include "Rendering/Gui.hpp"

#include "Resource/ResourceManager.hpp"
#include "Resource/ResourceRegistry.hpp"

#include "WindowInput.hpp"

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
		physicsRunner = std::make_unique<PhysicsRunner>(*scene);

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

		//ResourceManager().Get().ImportResource("C:/Users/Joao Comini/Documents/Sponza/sponza_small.glb");

		auto sponza = ResourceManager::Get().LoadResource<Scene>(6027400946597176494);
		scene->Add(*sponza);

		auto light = scene->CreateEntity();
		scene->AddComponent<Component::DirectionalLight>(light);

		auto query = scene->Query<Component::Transform, Component::MeshRender>();

		for (auto entity : query)
		{
			auto [transform, _] = query.GetComponent(entity);

			transform.scale = glm::vec3{ 0.01, 0.01, 0.01 };

		}

		while (running)
		{
			window->Update();

			auto currentTime = std::chrono::high_resolution_clock::now();
			auto timestep = std::chrono::duration<float>(currentTime - lastTime);
			lastTime = currentTime;

			OnUpdate(timestep.count());

			scene->GetComponent<Component::Transform>(light).rotation *= glm::quat(glm::vec3(glm::radians(5.f) * timestep.count(), 0.f, 0.0f));

			scene->Update();

			auto& commandBuffer = Renderer::Get().Begin();

			Renderer::Get().Draw();

			//Gui::Get().Begin();

			//OnGui();

			//Gui::Get().End(commandBuffer);

			Renderer::Get().End();
		}
	}

	void Application::OnWindowResize(int width, int height)
	{
		auto query = scene->Query<Component::Camera>();

		for (auto entity : query)
		{
			SetCameraAspectRatio(entity);
		}
	}

	void Application::SetCameraAspectRatio(Entity::Id entity)
	{
		auto [height, width] = window->GetFramebufferSize();

		auto& comp = scene->GetComponent<Component::Camera>(entity);

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

	void Application::StartScene()
	{
		scene->Resume();

		physicsRunner->Start();
		scriptRunner->Start();
	}

	void Application::UpdateScene(float timestep)
	{
		scriptRunner->Update(timestep);
		physicsRunner->Update(timestep);
	}

	void Application::StopScene()
	{
		scriptRunner->Stop();
		physicsRunner->Stop();

		scene->Pause();
	}
}
