#pragma once

#include <entt/entt.hpp>

#include "Common/Uuid.hpp"

#include "SceneMixin.hpp"

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
		auto TryGetComponent() const
		{
			return registry->try_get<T>(handle);
		}

		template<typename T>
		bool HasComponent() const
		{
			return registry->any_of<T>(handle);
		}

		template<typename T>
		decltype(auto) GetComponent() const
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

		template <class Archive>
		void Serialize(Archive& ar)
		{
			ar(handle);
		}

		void SetParent(Entity parent);
		Entity GetParent() const;

		void SetName(const std::string& name);
		std::string_view GetName() const;

		std::vector<Entity> GetChildren() const;
	private:
		entt::entity handle{ entt::null };
		entt::registry* registry = nullptr;

		void AddChild(Entity entity);
		void RemoveChild(Entity entity);
	};
};