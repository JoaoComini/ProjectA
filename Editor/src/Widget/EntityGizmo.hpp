#pragma once

#include <Scene/Entity.hpp>

#include <imgui.h>
#include <ImGuizmo.h>

#include "Widget.hpp"
#include "../EditorCamera.hpp"

class EntityGizmo : public Widget
{
public:
	EntityGizmo(EditorCamera& camera);

    void Draw() override;

	void SetEntity(Engine::Entity entity);

private:
	EditorCamera& camera;
    Engine::Entity entity{};

	ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
};