#include "EntityInspector.hpp"

#include <misc/cpp/imgui_stdlib.h>

#include <Scene/Scene.hpp>

#include "Controls.hpp"

EntityInspector::EntityInspector(Engine::Scene& scene)
	: scene(scene)
{
}

void EntityInspector::SetEntity(Engine::Entity::Id entity)
{
	this->entity = entity;
}

void EntityInspector::Draw()
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
		AddComponentMenuItem<Engine::Component::Camera>("Camera");
		AddComponentMenuItem<Engine::Component::Transform>("Transform");
		AddComponentMenuItem<Engine::Component::MeshRender>("Mesh Render");
		AddComponentMenuItem<Engine::Component::DirectionalLight>("Directional Light");
		AddComponentMenuItem<Engine::Component::PointLight>("Point Light");
		AddComponentMenuItem<Engine::Component::SkyLight>("Sky Light");
		AddComponentMenuItem<Engine::Component::Script>("Script");
		AddComponentMenuItem<Engine::Component::PhysicsBody>("Physics Body");
		AddComponentMenuItem<Engine::Component::BoxShape>("Box Shape");
		AddComponentMenuItem<Engine::Component::SphereShape>("Sphere Shape");
		ImGui::EndPopup();
	}

	if (auto transform = scene.TryGetComponent<Engine::Component::Transform>(entity))
	{
		ComponentControlNode<Engine::Component::Transform>("Transform", transform);
	}

	if (auto camera = scene.TryGetComponent<Engine::Component::Camera>(entity))
	{
		ComponentControlNode<Engine::Component::Camera>("Camera", camera);
	}

	if (auto meshRender = scene.TryGetComponent<Engine::Component::MeshRender>(entity))
	{
		ComponentControlNode<Engine::Component::MeshRender>("Mesh Render", meshRender);
	}

	if (auto directionalLight = scene.TryGetComponent<Engine::Component::DirectionalLight>(entity))
	{
		ComponentControlNode<Engine::Component::DirectionalLight>("Directional Light", directionalLight);
	}

	if (auto pointLight = scene.TryGetComponent<Engine::Component::PointLight>(entity))
	{
		ComponentControlNode<Engine::Component::PointLight>("Point Light", pointLight);
	}

	if (auto skyLight = scene.TryGetComponent<Engine::Component::SkyLight>(entity))
	{
		ComponentControlNode<Engine::Component::SkyLight>("Sky Light", skyLight);
	}

	if (auto script = scene.TryGetComponent<Engine::Component::Script>(entity))
	{
		ComponentControlNode<Engine::Component::Script>("Script", script);
	}

	if (auto body = scene.TryGetComponent<Engine::Component::PhysicsBody>(entity))
	{
		ComponentControlNode<Engine::Component::PhysicsBody>("Physics Body", body);
	}

	if (auto shape = scene.TryGetComponent<Engine::Component::BoxShape>(entity))
	{
		ComponentControlNode<Engine::Component::BoxShape>("Box Shape", shape);
	}

	if (auto shape = scene.TryGetComponent<Engine::Component::SphereShape>(entity))
	{
		ComponentControlNode<Engine::Component::SphereShape>("Sphere Shape", shape);
	}

	ImGui::End();
}
