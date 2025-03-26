#include "Editor.h"

#include <imgui.h>

#include "Resource/Importer/GltfModule.h"
#include "Resource/ResourceManager.h"

#include "Platform/FileDialog.h"
#include "Project/Project.h"

namespace Engine
{
    Editor::Editor(ApplicationSpec& spec) : Application(spec)
    {
		if (const auto path = spec.args[1]; std::filesystem::exists(path))
		{
			Project::Load(path);
		}
		else
		{
			Exit();
			return;
		}

		ResourceRegistry::Get().Deserialize();

		{
			auto gltf = std::make_unique<GltfModule>();

			auto scene = std::make_unique<SceneImporter>();
			scene->AddModule(std::move(gltf));

			ResourceManager::Get().AddImporter(std::move(scene));
		}

		auto [height, width] = GetWindow().GetFramebufferSize();
		camera = std::make_unique<EditorCamera>(glm::radians(60.f), (float)width / height, 0.1f, 1000.f);

		sceneHierarchy = std::make_unique<SceneHierarchy>();
		entityInspector = std::make_unique<EntityInspector>();
		mainMenuBar = std::make_unique<MainMenuBar>();
		contentBrowser = std::make_unique<ContentBrowser>(GetRenderContext().GetDevice());
		toolbar = std::make_unique<Toolbar>();
		viewportDragDrop = std::make_unique<ViewportDragDrop>();
		entityGizmo = std::make_unique<EntityGizmo>(*camera);

		sceneHierarchy->OnSelectEntity([&](auto entity) {
			entityInspector->SetEntity(entity);
			entityGizmo->SetEntity(entity);
		});

		mainMenuBar->OnExit([&]() {
			Exit();
		});

		mainMenuBar->OnImport([&]() {
			ImportFile();
		});

		mainMenuBar->OnSaveScene([&]() {
			SaveScene();
		});

		mainMenuBar->OnNewScene([&]() {
			NewScene();
		});

		contentBrowser->OnResourceDoubleClick([&](auto id, auto mapping) {
			switch (mapping.type)
			{
			case ResourceType::Scene:
				OpenScene(id);
				break;
			default:
				break;
			}
		});

		viewportDragDrop->OnDropResource([&](auto id, auto mapping)
		{
			switch (mapping.type)
			{
			case ResourceType::Scene:
				AddScene(id);
				break;
			case ResourceType::Cubemap:
				AddSkyLightToScene(id);
				break;
			default:
				break;
			}
		});

		toolbar->OnPlay([&]() {
			entityGizmo->SetEntity({});
			// TODO: copy scene state

			// sceneCopy = GetScene();

			StartScene();
		});

		toolbar->OnStop([&]() {
			StopScene();
			// SetScene(sceneCopy);
			entityInspector->SetEntity({});
			entityGizmo->SetEntity({});
		});

		fileWatcher = std::make_unique<FileWatcher>(Project::GetResourceDirectory());
		fileWatcher->OnUpdate([](auto path) {
			auto id = ResourceRegistry::Get().FindResourceByPath(path);

			if (!id)
			{
				return;
			}

			auto mapping = ResourceRegistry::Get().FindMappingById(id);

			// only reload scripts for now
			if (!mapping || mapping->type != ResourceType::Script)
			{
				return;
			}

			ResourceManager::Get().UnloadResource(id);
		});

		GetSceneGraph().Pause();
    }

	Editor::~Editor()
	{
		GetRenderContext().GetDevice().WaitIdle();
	}

	void Editor::OnUpdate(float timestep)
	{
		auto& scene = GetSceneGraph();

		if (scene.IsPaused())
		{
			camera->Update(timestep);
			
			SetRenderCamera(*camera);
			return;
		}

		auto query = scene.Query<Component::Transform, Component::Camera>();
		auto entity = query.First();

		if (scene.Valid(entity))
		{
			// TODO: use camera component

			auto& camera = scene.GetComponent<Component::Camera>(entity).camera;
			auto transform = scene.GetComponent<Component::Transform>(entity).GetLocalMatrix();

			SetRenderCamera({});
		}

		UpdateScene(timestep);
	}

    void Editor::OnGui()
    {
		auto& scene = GetSceneGraph();

		mainMenuBar->Draw(scene);
		toolbar->Draw(scene);
    	viewportDragDrop->Draw(scene);
		sceneHierarchy->Draw(scene);
		entityInspector->Draw(scene);
		contentBrowser->Draw(scene);
		entityGizmo->Draw(scene);
    }

	void Editor::OnWindowResize(int width, int height)
	{
		Application::OnWindowResize(width, height);
		camera->SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	}

	void Editor::OnInputEvent(const InputEvent& event)
	{
		camera->OnInputEvent(event);
	}

	void Editor::NewScene()
	{
    	auto& graph = GetSceneGraph();
    	graph.Clear();

		currentScene = ResourceId{ 0 };

		entityInspector->SetEntity({});
		entityGizmo->SetEntity({});
	}

	void Editor::SaveScene()
	{
    	auto& graph = GetSceneGraph();

    	auto scene = graph.Pack();

		if (! currentScene)
		{
			currentScene = ResourceManager::Get().CreateResource<SceneResource>("untitled.scene", *scene);
			return;
		}

		ResourceManager::Get().SaveResource<SceneResource>(currentScene, *scene);

		contentBrowser->RefreshResourceTree();
	}

	void Editor::AddScene(const ResourceId id)
	{
		const auto scene = ResourceManager::Get().LoadResource<SceneResource>(id);

		GetSceneGraph().Add(*scene);
	}

	void Editor::OpenScene(ResourceId id)
	{
    	auto resource = ResourceManager::Get().LoadResource<SceneResource>(id);

		auto& graph = GetSceneGraph();
    	graph.Clear();
    	graph.Add(*resource);

    	currentScene = id;

		entityInspector->SetEntity({});
		entityGizmo->SetEntity({});
	}

	void Editor::AddSkyLightToScene(ResourceId id)
	{
		const auto entity = GetSceneGraph().CreateEntity();
		GetSceneGraph().AddComponent<Component::SkyLight>(entity, id);
	}

	void Editor::ImportFile()
	{
		const std::filesystem::path file = FileDialog::OpenFile(GetWindow(), "Resource Files (*.glb)\0*.glb\0");

		if (file.empty())
		{
			return;
		}

    	const auto destination = Project::GetResourceDirectory() / file.filename();
    	std::filesystem::copy(file, destination);

		ResourceManager::Get().ImportResource(destination);
		contentBrowser->RefreshResourceTree();
	}
}
