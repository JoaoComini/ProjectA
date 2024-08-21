#include "EntityInspector.hpp"

#include <misc/cpp/imgui_stdlib.h>

#include "Controls.hpp"

void EntityInspector::SetEntity(Engine::Entity entity)
{
	this->entity = entity;
}

void EntityInspector::Draw()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSizeConstraints({ 350, 0 }, viewport->Size);

	ImGui::Begin("Inspector");

	if (!entity)
	{
		ImGui::End();
		return;
	}

	auto name = entity.TryGetComponent<Engine::Component::Name>();

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

	if (auto transform = entity.TryGetComponent<Engine::Component::Transform>())
	{
		ComponentControlNode<Engine::Component::Transform>("Transform", transform);
	}

	if (auto camera = entity.TryGetComponent<Engine::Component::Camera>())
	{
		ComponentControlNode<Engine::Component::Camera>("Camera", camera);
	}

	if (auto meshRender = entity.TryGetComponent<Engine::Component::MeshRender>())
	{
		ComponentControlNode<Engine::Component::MeshRender>("Mesh Render", meshRender);
	}

	if (auto directionalLight = entity.TryGetComponent<Engine::Component::DirectionalLight>())
	{
		ComponentControlNode<Engine::Component::DirectionalLight>("Directional Light", directionalLight);
	}

	if (auto pointLight = entity.TryGetComponent<Engine::Component::PointLight>())
	{
		ComponentControlNode<Engine::Component::PointLight>("Point Light", pointLight);
	}

	if (auto skyLight = entity.TryGetComponent<Engine::Component::SkyLight>())
	{
		ComponentControlNode<Engine::Component::SkyLight>("Sky Light", skyLight);
	}

	if (auto script = entity.TryGetComponent<Engine::Component::Script>())
	{
		ComponentControlNode<Engine::Component::Script>("Script", script);
	}

	if (auto body = entity.TryGetComponent<Engine::Component::PhysicsBody>())
	{
		ComponentControlNode<Engine::Component::PhysicsBody>("Physics Body", body);
	}

	if (auto shape = entity.TryGetComponent<Engine::Component::BoxShape>())
	{
		ComponentControlNode<Engine::Component::BoxShape>("Box Shape", shape);
	}

	if (auto shape = entity.TryGetComponent<Engine::Component::SphereShape>())
	{
		ComponentControlNode<Engine::Component::SphereShape>("Sphere Shape", shape);
	}

	ImGui::End();
}
