#pragma once

#include "Widget.h"
#include "../EditorCamera.h"

#include <imgui.h>
#include <ImGuizmo.h>

class EntityGizmo : public Widget
{
public:
	explicit EntityGizmo(EditorCamera& camera);

    void Draw(Engine::Scene& scene) override;

	void SetEntity(Engine::Entity::Id entity);

private:
	EditorCamera& camera;
    Engine::Entity::Id entity{ Engine::Entity::Null };

	ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
};