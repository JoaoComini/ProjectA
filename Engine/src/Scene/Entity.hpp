#pragma once

#include <entt/entt.hpp>

namespace Engine
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, entt::registry* registry);

		template<typename T, typename... Args>
		decltype(auto) AddComponent(Args&&... args)
		{
			return registry->emplace<T>(handle, std::forward<Args>(args)...);
		}

		template<typename T>
		auto TryGetComponent()
		{
			return registry->try_get<T>(handle);
		}

		template<typename T>
		decltype(auto) GetComponent()
		{
			return registry->get<T>(handle);
		}

		operator bool() const
		{ 
			return handle != entt::null;
		}

		bool operator==(const Entity& other) const
		{
			return handle == other.handle && registry == other.registry;
		}

	private:
		entt::entity handle{ entt::null };
		entt::registry* registry;
	};
};