#include "Editor.hpp"

#include <imgui.h>

namespace Engine
{
    Editor::Editor(ApplicationSpec& spec) : Application(spec)
    {
        AddSystem<CameraSystem>();

		sceneHierarchy = std::make_unique<SceneHierarchy>(GetScene());
		entityInspector = std::make_unique<EntityInspector>();

		sceneHierarchy->OnSelectEntity([&](auto entity) {
			entityInspector->SetEntity(entity);
		});
    }

    void Editor::OnGui()
    {
		MainMenuBar();

		sceneHierarchy->Update();
		entityInspector->Update();

		if (openMetricsWindow)
		{
			ImGuiIO& io = ImGui::GetIO();
			
			if (ImGui::Begin("Metrics", &openMetricsWindow))
			{
				ImGui::Text("Frametime: %.3f ms/frame", 1000.0f / io.Framerate);
				ImGui::Text("Framerate: %.1f FPS", io.Framerate);
				ImGui::End();
			}
		}
    }

	void Editor::MainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					Exit();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Metrics"))
				{
					openMetricsWindow = true;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}

