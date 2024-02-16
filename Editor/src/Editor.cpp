#include "Editor.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

#include "Resource/ResourceManager.hpp"
#include "Resource/Prefab.hpp"
#include "Rendering/Cubemap.hpp"
#include "Rendering/Renderer.hpp"

#include "Platform/FileDialog.hpp"
#include "Project/Project.hpp"
#include "Scene/Node/SkyLightNode.hpp"

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
		nodeInspector = std::make_unique<NodeInspector>();
		mainMenuBar = std::make_unique<MainMenuBar>();
		contentBrowser = std::make_unique<ContentBrowser>(Renderer::Get().GetRenderContext().GetDevice(), GetScene());
		viewportDragDrop = std::make_unique<ViewportDragDrop>();
		transformNodeGizmo = std::make_unique<TransformNodeGizmo>(*camera);

		sceneHierarchy->OnSelectNode([&](Node* node) {
			nodeInspector->SetNode(node);
			transformNodeGizmo->SetTransformNode(dynamic_cast<TransformNode*>(node));
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

		viewportDragDrop->OnDropResource([&](auto id, auto metadata)
		{
			switch (metadata.type)
			{
			case ResourceType::Scene:
				OpenScene(id);
				break;
			case ResourceType::Prefab:
				AddPrefabToScene(id);
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
		nodeInspector->Draw();
		contentBrowser->Draw();
		transformNodeGizmo->Draw();
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
		nodeInspector->SetNode(nullptr);
		transformNodeGizmo->SetTransformNode(nullptr);
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
		
		OpenScene(id);
		contentBrowser->RefreshResourceTree();
	}

	void Editor::OpenScene(ResourceId id)
	{
		auto scene = ResourceManager::Get().LoadResource<Scene>(id);

		nodeInspector->SetNode(nullptr);
		transformNodeGizmo->SetTransformNode(nullptr);
	}

	void Editor::AddPrefabToScene(ResourceId id)
	{
		auto prefab = ResourceManager::Get().LoadResource<Prefab>(id);

		GetScene().GetRoot()->AddChild(prefab->Instantiate());
	}

	void Editor::AddSkyLightToScene(ResourceId id)
	{
		auto node = new SkyLightNode();
		node->SetCubemap(id);

		GetScene().GetRoot()->AddChild(std::move(node));
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
