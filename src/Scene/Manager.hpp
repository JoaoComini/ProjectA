#pragma once

#include <entt/entt.hpp>

namespace Scene
{
	class Entity;

	class Manager
	{
	public:
		Manager() = default;
		~Manager() = default;

		Entity CreateEntity();

	private:
		entt::registry registry;
	};
};