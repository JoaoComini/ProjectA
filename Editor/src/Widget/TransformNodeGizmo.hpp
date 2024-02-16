#pragma once

#include <Scene/Node/TransformNode.hpp>

#include <imgui.h>
#include <ImGuizmo.h>

#include "Widget.hpp"
#include "../EditorCamera.hpp"

class TransformNodeGizmo : public Widget
{
public:
	TransformNodeGizmo(EditorCamera& camera);

    void Draw() override;

	void SetTransformNode(Engine::TransformNode* node);

private:
	EditorCamera& camera;
	Engine::TransformNode* node{ nullptr };

	ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
};