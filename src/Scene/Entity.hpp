#pragma once

#include <entt/entt.hpp>

namespace Scene
{
	class Entity
	{
	public:
		Entity(entt::entity handle, entt::registry* registry);

	private:
		entt::entity handle{ entt::null };
		entt::registry* registry;
	};
};