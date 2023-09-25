#pragma once

#include "Scene/EntityManager.hpp"

namespace Engine {

	class System
	{
	public:
		System(EntityManager& entityManager) : entityManager(entityManager) {}

		virtual ~System() = default;

		virtual void Update(float timestep) {}

	protected:

		EntityManager& entityManager;
	};

}