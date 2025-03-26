#pragma once

#include <Scene/SceneGraph.h>

class Widget
{
public:
	virtual ~Widget() = default;
	virtual void Draw(Engine::SceneGraph& scene) {}
};