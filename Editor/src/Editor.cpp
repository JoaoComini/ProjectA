#include "Editor.hpp"

#include <imgui.h>

#include "Resource/ResourceManager.hpp"

#include "Platform/FileDialog.hpp"

namespace Engine
{
    Editor::Editor(ApplicationSpec& spec) : Application(spec)
    {
        AddSystem<CameraSystem>();

		sceneHierarchy = std::make_unique<SceneHierarchy>(GetScene());
		entityInspector = std::make_unique<EntityInspector>();
		mainMenuBar = std::make_unique<MainMenuBar>();

		sceneHierarchy->OnSelectEntity([&](auto entity) {
			entityInspector->SetEntity(entity);
		});

		mainMenuBar->OnExit([&]() {
			Exit();
		});

		mainMenuBar->OnImport([&]() {
			ImportFile();
		});
    }

    void Editor::OnGui()
    {
		mainMenuBar->Update();
		sceneHierarchy->Update();
		entityInspector->Update();
    }

	void Editor::ImportFile()
	{
		auto file = FileDialog::OpenFile(GetWindow(), "GLB File (*.glb)\0*.glb\0");

		if (!file.empty())
		{
			ResourceManager::GetInstance()->ImportResource(file);
		}
	}
}
