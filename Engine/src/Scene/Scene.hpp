#pragma once

#include <entt/entt.hpp>

#include "SceneMixin.hpp"
#include "Entity.hpp"
#include "Components.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Mesh.hpp"

#include "Resource/Resource.hpp"

namespace Engine
{
	static auto Predicate = [] (Entity e) { return true; };

	template<typename... Type>
	struct Exclusion final {
		explicit constexpr Exclusion() {}
	};

	class Scene : public Resource
	{
	public:
		Scene();
		Scene& operator=(const Scene& other);

		Entity CreateEntity();
		void DestroyEntity(Entity entity);
		void Update();

		void Add(const Scene& scene);

		void Pause();
		void Resume();
		bool IsPaused() const;

		Entity FindEntityByHandle(uint32_t handle);

		template<typename... Args, typename... Exclude, typename Func>
		void ForEachEntity(Func func, Exclusion<Exclude...> = Exclusion{})
		{
			if constexpr (sizeof...(Args) == 0u)
			{
				auto view = registry.view<entt::entity>(entt::exclude<Exclude...>);

				for (auto entity : view)
				{
					if (!registry.valid(entity))
					{
						continue;
					}

					func(Entity { entity, &registry });
				}
			}
			else
			{
				auto view = registry.view<Args...>(entt::exclude<Exclude...>);

				for (auto entity : view)
				{
					if (! registry.valid(entity))
					{
						continue;
					}

					func(Entity { entity, &registry });
				}
			}
		}

		template<typename... Args>
		void Clear()
		{
			registry.clear<Args...>();
		}

		template<typename... Args, typename P = decltype(Predicate)>
		Entity FindFirstEntity(P predicate = Predicate)
		{
			auto view = registry.view<Args...>();

			for (auto entity : view) {
				if (predicate(Entity{ entity, &registry }))
				{
					return { entity, &registry };
				}
			}

			return {};
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
				.template get<Component::SkyLight>(ar)
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
				.template get<Component::SkyLight>(ar)
				.template get<Component::Script>(ar)
				.template get<Component::PhysicsBody>(ar)
				.template get<Component::BoxShape>(ar)
				.template get<Component::SphereShape>(ar);
		}

		static ResourceType GetStaticType()
		{
			return ResourceType::Scene;
		}

		virtual ResourceType GetType() const override
		{
			return GetStaticType();
		}

		static std::string GetExtension()
		{
			return "scn";
		}

	private:
		void ComputeEntityLocalToWorld(const Component::LocalToWorld& parent, Entity entity);

		entt::registry registry;

		bool paused = false;
	};
};
