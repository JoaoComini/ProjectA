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

		template<typename T>
		T* TryGetComponent()
		{
			return registry->try_get<T>(handle);
		}

	private:
		entt::entity handle{ entt::null };
		entt::registry* registry;
	};
};