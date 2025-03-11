#include "Editor.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>
#include <Resource/Importer/GltfImporter.hpp>

#include "Resource/ResourceManager.hpp"
#include "Rendering/Cubemap.hpp"

#include "Platform/FileDialog.hpp"
#include "Project/Project.hpp"

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

		{
			auto importer = std::make_unique<GltfImporter>();
			ResourceManager::Get().AddImporter(std::move(importer));
		}

		auto [height, width] = GetWindow().GetFramebufferSize();
		camera = std::make_unique<EditorCamera>(glm::radians(60.f), (float)width / height, 0.1f, 1000.f);

		sceneHierarchy = std::make_unique<SceneHierarchy>(GetScene());
		entityInspector = std::make_unique<EntityInspector>(GetScene());
		mainMenuBar = std::make_unique<MainMenuBar>();
		contentBrowser = std::make_unique<ContentBrowser>(GetRenderContext().GetDevice(), GetScene());
		toolbar = std::make_unique<Toolbar>(GetScene());
		viewportDragDrop = std::make_unique<ViewportDragDrop>();
		entityGizmo = std::make_unique<EntityGizmo>(GetScene(), *camera);

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

		contentBrowser->OnResourceDoubleClick([&](auto id, auto metadata) {
			switch (metadata.type)
			{
			case ResourceType::Scene:
				OpenScene(id);
				break;
			default:
				break;
			}
		});

		viewportDragDrop->OnDropResource([&](auto id, auto metadata)
		{
			switch (metadata.type)
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
			sceneCopy = GetScene();
			StartScene();
		});

		toolbar->OnStop([&]() {
			StopScene();
			SetScene(sceneCopy);
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

			auto metadata = ResourceRegistry::Get().FindMetadataById(id);

			// only reload scripts for now
			if (!metadata || metadata->type != ResourceType::Script)
			{
				return;
			}

			ResourceManager::Get().UnloadResource(id);
		});

		GetScene().Pause();
    }

	Editor::~Editor()
	{
		GetRenderContext().GetDevice().WaitIdle();
	}

	void Editor::OnUpdate(float timestep)
	{
		auto& scene = GetScene();

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
			auto& camera = scene.GetComponent<Component::Camera>(entity).camera;
			auto transform = scene.GetComponent<Component::Transform>(entity).GetLocalMatrix();

			SetRenderCamera({});
		}

		UpdateScene(timestep);
	}

    void Editor::OnGui()
    {
		mainMenuBar->Draw();
		toolbar->Draw();

		ImGuiIO& io = ImGui::GetIO();
		
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		auto id = ImGui::DockSpaceOverViewport(viewport, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode);

		DrawViewportDragDrop(id);

		sceneHierarchy->Draw();
		entityInspector->Draw();
		contentBrowser->Draw();
		entityGizmo->Draw();
    }

	void Editor::DrawViewportDragDrop(ImGuiID dockId)
	{
		if (ImGui::GetDragDropPayload() == nullptr)
		{
			return;
		}

		auto node = ImGui::DockBuilderGetCentralNode(dockId);

		ImGui::SetNextWindowSize(node->Size);
		ImGui::SetNextWindowPos(node->Pos);

		viewportDragDrop->Draw();
	}

	void Editor::OnWindowResize(int width, int height)
	{
		Application::OnWindowResize(width, height);
		camera->SetAspectRatio((float)width / height);
	}

	void Editor::OnInputEvent(const InputEvent& event)
	{
		camera->OnInputEvent(event);
	}

	void Editor::NewScene()
	{
		Scene scene;
		SetScene(scene);

		entityInspector->SetEntity({});
		entityGizmo->SetEntity({});
	}

	void Editor::SaveScene()
	{
		auto id = GetScene().id;
		
		if (id)
		{
			ResourceManager::Get().SaveResource<Scene>(id, GetScene());
		}
		else
		{
			id = ResourceManager::Get().CreateResource<Scene>("untitled", GetScene());
		}
		
		contentBrowser->RefreshResourceTree();
	}

	void Editor::AddScene(ResourceId id)
	{
		auto scene = ResourceManager::Get().LoadResource<Scene>(id);

		GetScene().Add(*scene);
	}

	void Editor::OpenScene(ResourceId id)
	{
		auto scene = ResourceManager::Get().LoadResource<Scene>(id);

		SetScene(*scene);

		entityInspector->SetEntity({});
		entityGizmo->SetEntity({});
	}

	void Editor::AddSkyLightToScene(ResourceId id)
	{
		auto entity = GetScene().CreateEntity();
		GetScene().AddComponent<Component::SkyLight>(entity, id);
	}

	void Editor::ImportFile()
	{
		auto file = FileDialog::OpenFile(GetWindow(), "Resource Files (*.glb,*.hdr)\0*.glb;*.hdr\0");

		if (file.empty())
		{
			return;
		}

		ResourceManager::Get().ImportResource(file);
		contentBrowser->RefreshResourceTree();
	}
}
