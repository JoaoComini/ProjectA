#include "Application.hpp"

#include "glm/gtc/random.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "GLFW/glfw3.h"

#include "Scene/Scene.hpp"
#include "Scene/Components.hpp"

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

		window->OnInputEvent([this](const auto& event) {
			bool captured = gui->OnInputEvent(event);

			if (captured)
			{
				return;
			}

			OnInputEvent(event);
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

		renderContext = std::make_unique<RenderContext>(*window);
		renderer = std::make_unique<Renderer>(*renderContext);
		gui = std::make_unique<Gui>(*window, *renderContext);

		scene = std::make_unique<Scene>();
		scene->OnComponentAdded<Component::Camera, &Application::SetCameraAspectRatio>(this);

		scriptRunner = std::make_unique<ScriptRunner>(*scene);
		physicsRunner = std::make_unique<PhysicsRunner>(*scene);

		ResourceManager::Create(*renderContext);
		ResourceRegistry::Create();
		Input::Create<WindowInput>(*window);

		running = true;
	}

	Application::~Application()
	{
		gui.reset();

		Container::TearDown();
	}

	void Application::Run()
	{
		auto lastTime = std::chrono::high_resolution_clock::now();

		while (running)
		{
			window->Update();

			auto currentTime = std::chrono::high_resolution_clock::now();
			auto timestep = std::chrono::duration<float>(currentTime - lastTime);
			lastTime = currentTime;

			OnUpdate(timestep.count());

			scene->Update();

			auto* commandBuffer = renderContext->Begin();

			auto& frame = renderContext->GetCurrentFrame();
			auto& target = frame.GetTarget();
			auto& attachment = target.GetColorAttachment(0);

			renderer->Draw(*commandBuffer, *scene, camera, attachment);

			gui->Begin();

			OnGui();

			gui->Draw(*commandBuffer, attachment);

			renderContext->End(*commandBuffer);
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

	RenderContext& Application::GetRenderContext()
	{
		return *renderContext;
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
