#pragma once

#include <Scene/Scene.hpp>

#include "Widget.hpp"

class SceneHierarchy: public Widget
{
public:
	SceneHierarchy(Engine::Scene& scene);
	~SceneHierarchy() = default;

	void Draw() override;

	void OnSelectEntity(std::function<void(Engine::Entity)> onSelectEntityFn);

private:
	void EntityNode(Engine::Entity entity, bool root);

	Engine::Scene& scene;
	Engine::Entity selectedEntity;
	std::function<void(Engine::Entity)> onSelectEntityFn;
};