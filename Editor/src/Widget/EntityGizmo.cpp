#include "EntityGizmo.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <Scene/Scene.hpp>

EntityGizmo::EntityGizmo(Engine::Scene& scene, EditorCamera& camera) : scene(scene), camera(camera)
{
}

void EntityGizmo::Draw()
{
	if (!scene.Valid(entity))
	{
		return;
	}

	auto transform = scene.TryGetComponent<Engine::Component::Transform>(entity);

	if (! transform)
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

	auto matrix = scene.GetComponent<Engine::Component::LocalToWorld>(entity).value;
	glm::mat4 delta{ 0.f };

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), operation, ImGuizmo::MODE::LOCAL, glm::value_ptr(matrix));

	if (!ImGuizmo::IsUsing())
	{
		return;
	}

	auto local = matrix;

	if (auto hierarchy = scene.TryGetComponent<Engine::Component::Hierarchy>(entity))
	{
		auto matrix = scene.GetComponent<Engine::Component::LocalToWorld>(hierarchy->parent).value;

		local = glm::inverse(matrix) * local;
	}


	glm::vec3 position{};
	glm::vec3 rotation{};
	glm::vec3 scale{};

	ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(local), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));

	transform->position = position;
	transform->rotation = glm::quat(glm::radians(rotation));
	transform->scale = scale;
}

void EntityGizmo::SetEntity(Engine::Entity::Id entity)
{
	this->entity = entity;
}
