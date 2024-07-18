#include "Editor.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

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

		auto [height, width] = GetWindow().GetFramebufferSize();
		camera = std::make_unique<EditorCamera>(glm::radians(60.f), (float)width / height, 0.1f, 2000.f);

		sceneHierarchy = std::make_unique<SceneHierarchy>(GetScene());
		entityInspector = std::make_unique<EntityInspector>();
		mainMenuBar = std::make_unique<MainMenuBar>();
		contentBrowser = std::make_unique<ContentBrowser>(Renderer::Get().GetRenderContext().GetDevice(), GetScene());
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
    }

	Editor::~Editor()
	{
		Renderer::Get().GetRenderContext().GetDevice().WaitIdle();
	}

	void Editor::OnUpdate(float timestep)
	{
		camera->Update(timestep);
		auto transform = camera->GetTransform();

		Renderer::Get().SetMainCamera(*camera, transform);
	}

    void Editor::OnGui()
    {
		ImGuiIO& io = ImGui::GetIO();
		
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		auto id = ImGui::DockSpaceOverViewport(viewport, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode);

		DrawViewportDragDrop(id);

		mainMenuBar->Draw();
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
		entity.AddComponent<Component::SkyLight>(id);
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
