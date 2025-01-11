#pragma once

#include <Scene/Entity.hpp>

#include "Widget.hpp"

namespace Engine
{
	class Scene;
}

class SceneHierarchy: public Widget
{
public:
	SceneHierarchy(Engine::Scene& scene);
	~SceneHierarchy() = default;

	void Draw() override;

	void OnSelectEntity(std::function<void(Engine::Entity::Id)> onSelectEntityFn);

private:
	void EntityNode(Engine::Entity::Id entity);

	Engine::Scene& scene;
	Engine::Entity::Id selectedEntity{ Engine::Entity::Null };
	std::function<void(Engine::Entity::Id)> onSelectEntityFn;
};