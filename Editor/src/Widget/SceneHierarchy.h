#pragma once

#include "Widget.h"

class SceneHierarchy : public Widget
{
public:
	void Draw(Engine::Scene& scene) override;

	void OnSelectEntity(std::function<void(Engine::Entity::Id)> onSelectEntityFn);

private:
	void EntityNode(Engine::Scene& scene, Engine::Entity::Id entity);

	Engine::Entity::Id selectedEntity{ Engine::Entity::Null };
	std::function<void(Engine::Entity::Id)> onSelectEntityFn;
};