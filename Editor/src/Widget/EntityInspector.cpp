#include "EntityInspector.hpp"

#include <misc/cpp/imgui_stdlib.h>

#include <glm/glm.hpp>

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
		ImGui::EndPopup();
	}

	auto transform = entity.TryGetComponent<Engine::Component::Transform>();

	if (transform)
	{
		ComponentControlNode<Engine::Component::Transform>("Transform", transform);
	}

	auto camera = entity.TryGetComponent<Engine::Component::Camera>();

	if (camera)
	{
		ComponentControlNode<Engine::Component::Camera>("Camera", camera);
	}

	auto meshRender = entity.TryGetComponent<Engine::Component::MeshRender>();

	if (meshRender)
	{
		ComponentControlNode<Engine::Component::MeshRender>("Mesh Render", meshRender);
	}

	auto directionalLight = entity.TryGetComponent<Engine::Component::DirectionalLight>();

	if (directionalLight)
	{
		ComponentControlNode<Engine::Component::DirectionalLight>("Directional Light", directionalLight);
	}

	auto pointLight = entity.TryGetComponent<Engine::Component::PointLight>();

	if (pointLight)
	{
		ComponentControlNode<Engine::Component::PointLight>("Point Light", pointLight);
	}

	ImGui::End();
}
