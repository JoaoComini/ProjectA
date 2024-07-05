#pragma once

#include <memory>
#include <vector>

#include "Scene/Scene.hpp"

#include "Window.hpp"

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

		Scene& GetScene();
		void SetScene(Scene& scene);

		Window& GetWindow();

		void SetCameraAspectRatio(Entity entity);

		std::unique_ptr<Window> window;
		std::unique_ptr<Scene> scene;

		bool running = false;
	};

	std::unique_ptr<Application> CreateApplication(ApplicationArgs args);
}