#pragma once

#include <Scene/Entity.hpp>

#include <imgui.h>
#include <ImGuizmo.h>

#include "Widget.hpp"
#include "../EditorCamera.hpp"

namespace Engine
{
	class Scene;
}

class EntityGizmo : public Widget
{
public:
	EntityGizmo(Engine::Scene& scene, EditorCamera& camera);

    void Draw() override;

	void SetEntity(Engine::Entity::Id entity);

private:
	Engine::Scene& scene;
	EditorCamera& camera;
    Engine::Entity::Id entity{ Engine::Entity::Null };

	ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
};