#include "EntityInspector.h"

#include <misc/cpp/imgui_stdlib.h>

#include "Controls.h"

void EntityInspector::SetEntity(Engine::Entity::Id entity)
{
	this->entity = entity;
}

void EntityInspector::Draw(Engine::Scene& scene)
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSizeConstraints({ 350, 0 }, viewport->Size);

	ImGui::Begin("Inspector");

	if (!scene.Valid(entity))
	{
		ImGui::End();
		return;
	}

	auto name = scene.TryGetComponent<Engine::Component::Name>(entity);

	if (name)
	{
		ImGui::InputText("##Name", &name->name);
	}

	ImGui::SameLine();

	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("AddComponent");
	}

	if (ImGui::BeginPopup("AddComponent"))
	{
		AddComponentMenuItem<Engine::Component::Camera>(scene,"Camera");
		AddComponentMenuItem<Engine::Component::Transform>(scene, "Transform");
		AddComponentMenuItem<Engine::Component::MeshRender>(scene, "Mesh Render");
		AddComponentMenuItem<Engine::Component::DirectionalLight>(scene, "Directional Light");
		AddComponentMenuItem<Engine::Component::PointLight>(scene, "Point Light");
		AddComponentMenuItem<Engine::Component::SkyLight>(scene, "Sky Light");
		AddComponentMenuItem<Engine::Component::Script>(scene, "Script");
		AddComponentMenuItem<Engine::Component::PhysicsBody>(scene, "Physics Body");
		AddComponentMenuItem<Engine::Component::BoxShape>(scene, "Box Shape");
		AddComponentMenuItem<Engine::Component::SphereShape>(scene, "Sphere Shape");
		ImGui::EndPopup();
	}

	if (auto transform = scene.TryGetComponent<Engine::Component::Transform>(entity))
	{
		ComponentControlNode<Engine::Component::Transform>(scene, "Transform", transform);
	}

	if (auto camera = scene.TryGetComponent<Engine::Component::Camera>(entity))
	{
		ComponentControlNode<Engine::Component::Camera>(scene, "Camera", camera);
	}

	if (auto meshRender = scene.TryGetComponent<Engine::Component::MeshRender>(entity))
	{
		ComponentControlNode<Engine::Component::MeshRender>(scene, "Mesh Render", meshRender);
	}

	if (auto directionalLight = scene.TryGetComponent<Engine::Component::DirectionalLight>(entity))
	{
		ComponentControlNode<Engine::Component::DirectionalLight>(scene, "Directional Light", directionalLight);
	}

	if (auto pointLight = scene.TryGetComponent<Engine::Component::PointLight>(entity))
	{
		ComponentControlNode<Engine::Component::PointLight>(scene, "Point Light", pointLight);
	}

	if (auto skyLight = scene.TryGetComponent<Engine::Component::SkyLight>(entity))
	{
		ComponentControlNode<Engine::Component::SkyLight>(scene, "Sky Light", skyLight);
	}

	if (auto script = scene.TryGetComponent<Engine::Component::Script>(entity))
	{
		ComponentControlNode<Engine::Component::Script>(scene, "Script", script);
	}

	if (auto body = scene.TryGetComponent<Engine::Component::PhysicsBody>(entity))
	{
		ComponentControlNode<Engine::Component::PhysicsBody>(scene, "Physics Body", body);
	}

	if (auto shape = scene.TryGetComponent<Engine::Component::BoxShape>(entity))
	{
		ComponentControlNode<Engine::Component::BoxShape>(scene, "Box Shape", shape);
	}

	if (auto shape = scene.TryGetComponent<Engine::Component::SphereShape>(entity))
	{
		ComponentControlNode<Engine::Component::SphereShape>(scene, "Sphere Shape", shape);
	}

	ImGui::End();
}
