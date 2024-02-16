#include "TransformNodeGizmo.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

TransformNodeGizmo::TransformNodeGizmo(EditorCamera& camera) : camera(camera)
{
}

void TransformNodeGizmo::Draw()
{
	if (!node)
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

	auto matrix = node->GetGlobalTransform().GetMatrix();

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), operation, ImGuizmo::MODE::LOCAL, glm::value_ptr(matrix));

	if (!ImGuizmo::IsUsing())
	{
		return;
	}

	Engine::Transform global{};
	global.origin = matrix[3];
	global.basis.matrix = matrix;

	node->SetGlobalTransform(global);
}

void TransformNodeGizmo::SetTransformNode(Engine::TransformNode* node)
{
	this->node = node;
}
