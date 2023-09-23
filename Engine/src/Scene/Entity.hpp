#pragma once

#include <entt/entt.hpp>

namespace Scene
{
	class Entity
	{
	public:
		Entity(entt::entity handle, entt::registry* registry);

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return registry->emplace<T>(handle, std::forward<Args>(args)...);
		}

	private:
		entt::entity handle{ entt::null };
		entt::registry* registry;
	};
};