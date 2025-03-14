#pragma once

#include <entt/entt.hpp>

#include "Entity.h"
#include "Components.h"

#include "Resource/Resource.h"

#include "SceneMixin.h"

namespace Engine
{
	template<typename...>
	struct Exclusion final {
		explicit constexpr Exclusion() = default;
	};

	template<typename, typename>
	struct SceneQuery;

	template<typename... Get, typename... Exclude>
	struct SceneQuery<entt::type_list<Get...>, entt::type_list<Exclude...>>
	{
		using Iterator = typename entt::view<entt::get_t<Get...>, entt::exclude_t<Exclude...>>::iterator;

		explicit SceneQuery(entt::registry& registry)
		{
			view = registry.view<Get...>(entt::exclude<Exclude...>);
		}

		template<typename T>
		decltype(auto) TryGetComponent(Entity::Id id) const
		{
			return view.template try_get<T>(id);
		}

		template<typename T>
		[[nodiscard]] bool HasComponent(Entity::Id id) const
		{
			return view.template any_of<T>(id);
		}

		template<typename... T>
		decltype(auto) GetComponent(Entity::Id id) const
		{
			return view.get<T...>(id);
		}

		[[nodiscard]] Entity::Id First() const
		{
			return view.front();
		}

		Iterator begin() const noexcept {
			return view.begin();
		}

		Iterator end() const noexcept {
			return view.end();
		}

	private:
		entt::view<entt::get_t<Get...>, entt::exclude_t<Exclude...>> view;
	};

	class Scene final : public Resource
	{
	public:
		Scene();
		Scene& operator=(const Scene& other);

		Entity::Id CreateEntity();
		void DestroyEntity(Entity::Id entity);

		[[nodiscard]] bool Valid(Entity::Id entity) const;

		void SetParent(Entity::Id entity, Entity::Id parent);
		Entity::Id GetParent(Entity::Id entity);

		void Update();

		void Add(const Scene& scene);

		void Pause();
		void Resume();
		[[nodiscard]] bool IsPaused() const;

		template<typename... Get, typename... Exclude>
		decltype(auto) Query(Exclusion<Exclude...> = Exclusion{})
		{
			return SceneQuery<entt::type_list<Get...>, entt::type_list<Component::Delete, Exclude...>>(registry);
		}

		template<typename... Args>
		void Clear()
		{
			registry.clear<Args...>();
		}

		template<typename T, typename... Args>
		decltype(auto) AddComponent(Entity::Id entity, Args&&... args)
		{
			return registry.emplace<T>(entity, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		decltype(auto) AddOrReplaceComponent(Entity::Id entity, Args&&... args)
		{
			return registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
		}

		template<typename... T>
		decltype(auto) GetComponent(Entity::Id id)
		{
			return registry.get<T...>(id);
		}

		template<typename T, typename... Args>
		decltype(auto) GetOrAddComponent(Entity::Id id, Args&&... args)
		{
			return registry.get_or_emplace<T>(id, std::forward<Args>(args)...);
		}

		template<typename... T>
		auto TryGetComponent(Entity::Id id)
		{
			return registry.try_get<T...>(id);
		}

		template<typename T>
		[[nodiscard]] bool HasComponent(Entity::Id id) const
		{
			return registry.any_of<T>(id);
		}

		template<typename... T>
		void RemoveComponent(Entity::Id id)
		{
			registry.remove<T...>(id);
		}
		
		template<typename T, auto FreeFunc>
		void OnComponentAdded()
		{
			registry.on_construct<T>().template connect<FreeFunc>();
		}

		template<typename T, auto MemberFunc, typename Type>
		void OnComponentAdded(Type instance)
		{
			registry.on_construct<T>().template connect<MemberFunc>(instance);
		}

		template<class Archive>
		void Save(Archive& ar) const
		{
			entt::snapshot snapshot{ registry };

			snapshot
				.get<entt::entity>(ar)
				.template get<Component::Name>(ar)
				.template get<Component::Children>(ar)
				.template get<Component::Hierarchy>(ar)
				.template get<Component::Transform>(ar)
				.template get<Component::MeshRender>(ar)
				.template get<Component::Camera>(ar)
				.template get<Component::DirectionalLight>(ar)
				.template get<Component::PointLight>(ar)
				.template get<Component::Script>(ar)
				.template get<Component::PhysicsBody>(ar)
				.template get<Component::BoxShape>(ar)
				.template get<Component::SphereShape>(ar);
		}

		template<class Archive>
		void Load(Archive& ar)
		{
			entt::snapshot_loader loader{ registry };

			loader
				.get<entt::entity>(ar)
				.template get<Component::Name>(ar)
				.template get<Component::Children>(ar)
				.template get<Component::Hierarchy>(ar)
				.template get<Component::Transform>(ar)
				.template get<Component::MeshRender>(ar)
				.template get<Component::Camera>(ar)
				.template get<Component::DirectionalLight>(ar)
				.template get<Component::PointLight>(ar)
				.template get<Component::Script>(ar)
				.template get<Component::PhysicsBody>(ar)
				.template get<Component::BoxShape>(ar)
				.template get<Component::SphereShape>(ar);
		}

		static ResourceType GetStaticType()
		{
			return ResourceType::Scene;
		}

		[[nodiscard]] ResourceType GetType() const override
		{
			return GetStaticType();
		}

		static std::string GetExtension()
		{
			return "scn";
		}

	private:
		void ComputeEntityLocalToWorld(const Component::LocalToWorld& parent, Entity::Id entity);

		void AddChild(Entity::Id parent, Entity::Id child);
		void RemoveChild(Entity::Id parent, Entity::Id child);

		entt::registry registry;

		bool paused = false;
	};
};
