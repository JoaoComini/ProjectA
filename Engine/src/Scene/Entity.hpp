#pragma once

#include <entt/entt.hpp>

namespace Engine
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, entt::registry* registry);

		uint32_t GetHandle()
		{
			return static_cast<uint32_t>(handle);
		}

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
		bool HasComponent()
		{
			return registry->any_of<T>(handle);
		}

		template<typename T>
		decltype(auto) GetComponent()
		{
			return registry->get<T>(handle);
		}

		template<typename T>
		void RemoveComponent()
		{
			registry->remove<T>(handle);
		}

		operator bool() const
		{ 
			return handle != entt::null;
		}

		operator entt::entity() const
		{
			return handle;
		}

		bool operator==(const Entity& other) const
		{
			return handle == other.handle && registry == other.registry;
		}

		void SetParent(Entity parent);
		Entity GetParent();

		std::vector<Entity> GetChildren();

	private:
		entt::entity handle{ entt::null };
		entt::registry* registry = nullptr;

		void AddChild(Entity entity);
		void RemoveChild(Entity entity);
	};
};