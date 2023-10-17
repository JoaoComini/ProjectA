#include "Editor.hpp"

#include <imgui.h>

#include "Resource/ResourceManager.hpp"
#include "Platform/FileDialog.hpp"
#include "Project/Project.hpp"
#include "Scene/SceneSerializer.hpp"

namespace Engine
{
    Editor::Editor(ApplicationSpec& spec) : Application(spec)
    {
		if (auto path = spec.args[1]; std::filesystem::exists(path))
		{
			Project::Load(path);
		}
		else
		{
			Exit();
			return;
		}

		ResourceRegistry::Get().Deserialize();

		sceneHierarchy = std::make_unique<SceneHierarchy>(GetScene());
		entityInspector = std::make_unique<EntityInspector>();
		mainMenuBar = std::make_unique<MainMenuBar>();
		contentBrowser = std::make_unique<ContentBrowser>(GetDevice(), GetScene());

		sceneHierarchy->OnSelectEntity([&](auto entity) {
			entityInspector->SetEntity(entity);
		});

		mainMenuBar->OnExit([&]() {
			Exit();
		});

		mainMenuBar->OnImport([&]() {
			ImportFile();

			contentBrowser->RefreshResourceTree();
		});

		mainMenuBar->OnSaveScene([&]() {
			SaveScene();
		});

		mainMenuBar->OnOpenScene([&]() {
			OpenScene();
		});

		auto [width, height] = GetWindow().GetFramebufferSize();

		camera = std::make_unique<EditorCamera>(glm::radians(45.f), (float)width / height, 0.1f, 1000.f);
    }

	void Editor::OnUpdate(float timestep)
	{
		camera->Update(timestep);

		Renderer::Get().SetCamera(*camera, camera->GetTransform());
	}

    void Editor::OnGui()
    {
		ImGuiIO& io = ImGui::GetIO();
		
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::DockSpaceOverViewport(viewport);
		}

		mainMenuBar->Draw();
		sceneHierarchy->Draw();
		entityInspector->Draw();
		contentBrowser->Draw();
    }

	void Editor::OnWindowResize(int width, int height)
	{
		Application::OnWindowResize(width, height);

		camera->SetAspectRatio((float)width / height);
	}

	void Editor::SaveScene()
	{
		SceneSerializer serializer(GetScene());

		if (!scenePath.empty())
		{
			serializer.Serialize(scenePath);
			return;
		}

		std::string path = FileDialog::SaveFile(GetWindow(), "ProjectA Scene (*.pascn)\0*.pascn\0");

		if (path.empty())
		{
			return;
		}

		serializer.Serialize(path);
		scenePath = path;
	}

	void Editor::OpenScene()
	{
		std::string path = FileDialog::OpenFile(GetWindow(), "ProjectA Scene (*.pascn)\0*.pascn\0");

		if (path.empty())
		{
			return;
		}

		GetScene().Clear();

		SceneSerializer serializer(GetScene());

		serializer.Deserialize(path);
		scenePath = path;
	}

	void Editor::ImportFile()
	{
		auto file = FileDialog::OpenFile(GetWindow(), "GLB File (*.glb)\0*.glb\0");

		if (!file.empty())
		{
			ResourceManager::Get().ImportResource(file);
		}
	}
}
