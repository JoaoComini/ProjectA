#pragma once

#include <Scene/Scene.h>

class Widget
{
public:
	virtual ~Widget() = default;
	virtual void Draw(Engine::Scene& scene) {}
};