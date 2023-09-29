#pragma once

#include "Core/System.hpp"
#include "Core/Input.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Components.hpp"

class InputSystem: public Engine::System
{
public:
	InputSystem(Engine::Scene& scene);

	void Update(float timestep) override;

private:
	Engine::Input* input;

	Engine::Entity entity;

	float speed = 10.f;
};

