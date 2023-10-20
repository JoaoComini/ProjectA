#include "EntityGizmo.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <Scene/Components.hpp>

EntityGizmo::EntityGizmo(EditorCamera& camera) : camera(camera)
{
}

glm::mat4 GetEntityWorldMatrix(Engine::Entity entity)
{
	if (!entity)
	{
		return glm::mat4{ 1.0f };
	}

	auto parent = entity.GetParent();
	auto& transform = entity.GetComponent<Engine::Component::Transform>();

	if (!parent)
	{
		return transform.GetLocalMatrix();
	}

	auto parentTransform = parent.TryGetComponent<Engine::Component::Transform>();

	if (!parentTransform)
	{
		return transform.GetLocalMatrix();
	}

	return GetEntityWorldMatrix(parent) * transform.GetLocalMatrix();
}

void EntityGizmo::Draw()
{
	if (!entity)
	{
		return;
	}

	auto transform = entity.TryGetComponent<Engine::Component::Transform>();

	if (!transform)
	{
		return;
	}

	if (Engine::Input::Get().IsKeyDown(Engine::KeyCode::Q))
	{
		operation = ImGuizmo::OPERATION::TRANSLATE;
	}

	if (Engine::Input::Get().IsKeyDown(Engine::KeyCode::E))
	{
		operation = ImGuizmo::OPERATION::ROTATE;
	}

	if (Engine::Input::Get().IsKeyDown(Engine::KeyCode::R))
	{
		operation = ImGuizmo::OPERATION::SCALE;
	}

	auto view = glm::inverse(camera.GetTransform());
	auto projection = camera.GetProjection();
	projection[1][1] *= -1;

	auto matrix = GetEntityWorldMatrix(entity);

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), operation, ImGuizmo::MODE::LOCAL, glm::value_ptr(matrix));

	if (!ImGuizmo::IsUsing())
	{
		return;
	}

	auto local = glm::inverse(GetEntityWorldMatrix(entity.GetParent())) * matrix;

	glm::vec3 position{};
	glm::vec3 rotation{};
	glm::vec3 scale{};

	ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(local), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));

	transform->position = position;
	transform->scale = scale;
	transform->rotation = glm::quat(glm::radians(rotation));
}

void EntityGizmo::SetEntity(Engine::Entity entity)
{
	this->entity = entity;
}
