#pragma once

#include "Window.h"
#include "Gui.h"
#include "Scene/SceneGraph.h"
#include "Scripting/ScriptRunner.h"
#include "Physics/PhysicsRunner.h"
#include "Rendering/Renderer.h"

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

		SceneGraph& GetSceneGraph() const;

		void SetRenderCamera(RenderCamera camera)
		{
			this->camera = camera;
		}

		Window& GetWindow() const;

		RenderContext& GetRenderContext() const;

		void SetCameraAspectRatio(Entity::Id entity);
	private:
		std::unique_ptr<Window> window;
		std::unique_ptr<RenderContext> renderContext;
		std::unique_ptr<Renderer> renderer;
		std::unique_ptr<Gui> gui;

		std::unique_ptr<SceneGraph> scene;
		RenderCamera camera;

		std::unique_ptr<ScriptRunner> scriptRunner;
		std::unique_ptr<PhysicsRunner> physicsRunner;

		bool running = false;
	};

	std::unique_ptr<Application> CreateApplication(const ApplicationArgs &args);
}