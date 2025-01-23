#pragma once

#include "Window.hpp"
#include "Gui.hpp"
#include "Scene/Scene.hpp"
#include "Scripting/ScriptRunner.hpp"
#include "Physics/PhysicsRunner.hpp"
#include "Rendering/Renderer.hpp"

namespace Engine {

	struct ApplicationArgs
	{
		int argc = 0;
		std::vector<std::string> argv;

		const std::string operator[](int index) const
		{
			if (index >= argc)
			{
				return {};
			}

			return argv[index];
		}
	};

	struct ApplicationSpec
	{
		std::string name = "Engine Application";
		ApplicationArgs args{};
	};

	class Application
	{
	public:
		Application(ApplicationSpec &spec);
		virtual ~Application();

		void Run();
		void Exit();
	protected:
		virtual void OnUpdate(float timestep) {}
		virtual void OnGui() {}
		virtual void OnWindowResize(int width, int height);
		virtual void OnInputEvent(const InputEvent& event) {}

		void StartScene();
		void UpdateScene(float timestep);
		void StopScene();

		Scene& GetScene();
		void SetScene(Scene& scene);

		Window& GetWindow();

		void SetCameraAspectRatio(Entity::Id entity);
	private:
		std::unique_ptr<Window> window;
		std::unique_ptr<Gui> gui;
		std::unique_ptr<Scene> scene;
		std::unique_ptr<ScriptRunner> scriptRunner;
		std::unique_ptr<PhysicsRunner> physicsRunner;

		bool running = false;
	};

	std::unique_ptr<Application> CreateApplication(ApplicationArgs args);
}