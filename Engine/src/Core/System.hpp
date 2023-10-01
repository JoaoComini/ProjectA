#pragma once

#include "Scene/Scene.hpp"

namespace Engine {

	class System
	{
	public:
		System(Scene& scene) : scene(scene) {}

		virtual ~System() = default;

		virtual void Update(float timestep) {}

	protected:
		Scene& scene;
	};

}