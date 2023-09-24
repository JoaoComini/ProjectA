#pragma once

#include "Scene/EntityManager.hpp"

class System
{
public:
	System(Scene::EntityManager& entityManager): entityManager(entityManager) {}

	virtual ~System() = default;

	virtual void Update(float timestep) {}

protected:

	Scene::EntityManager& entityManager;
};