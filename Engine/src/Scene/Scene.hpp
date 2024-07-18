#pragma once

#include <entt/entt.hpp>

#include "SceneMixin.hpp"
#include "Entity.hpp"
#include "Components.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Mesh.hpp"

#include "Resource/Resource.hpp"

#include <vector>
#include <unordered_map>
#include <functional>

namespace Engine
{
	static auto Predicate = [] (Entity e) { return true; };

	class Scene : public Resource
	{
	public:
		Scene& operator=(const Scene& other);
		Scene& operator+(const Scene& other);

		Entity CreateEntity();
		void DestroyEntity(Entity entity);
		void Update();

		void Add(const Scene& scene);

		template<typename... Args, typename Func>
		void ForEachEntity(Func func)
		{
			if constexpr (sizeof...(Args) == 0u)
			{
				auto view = registry.view<entt::entity>();

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
				auto view = registry.view<Args...>();

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


		template<typename... Args, typename P = decltype(Predicate)>
		std::pair<Entity, bool> FindFirstEntity(P predicate = Predicate)
		{
			auto view = registry.view<Args...>();

			for (auto entity : view) {
				if (predicate({ entity, &registry }))
				{
					return { { entity, &registry }, true };
				}
			}

			return { {}, false };
		}
		
		template<typename T, auto FreeFunc>
		void OnComponentAdded()
		{
			registry.on_construct<T>().connect<FreeFunc>();
		}

		template<typename T, auto MemberFunc, typename Type>
		void OnComponentAdded(Type instance)
		{
			registry.on_construct<T>().connect<MemberFunc>(instance);
		}

		template<class Archive>
		void Save(Archive& ar) const
		{
			entt::snapshot snapshot{ registry };

			snapshot
				.get<entt::entity>(ar)
				.get<Component::Name>(ar)
				.get<Component::Relationship>(ar)
				.get<Component::Transform>(ar)
				.get<Component::MeshRender>(ar)
				.get<Component::Camera>(ar)
				.get<Component::DirectionalLight>(ar)
				.get<Component::PointLight>(ar)
				.get<Component::SkyLight>(ar);
		}

		template<class Archive>
		void Load(Archive& ar)
		{
			entt::snapshot_loader loader{ registry };

			loader
				.get<entt::entity>(ar)
				.get<Component::Name>(ar)
				.get<Component::Relationship>(ar)
				.get<Component::Transform>(ar)
				.get<Component::MeshRender>(ar)
				.get<Component::Camera>(ar)
				.get<Component::DirectionalLight>(ar)
				.get<Component::PointLight>(ar)
				.get<Component::SkyLight>(ar);
		}

		static ResourceType GetStaticType()
		{
			return ResourceType::Scene;
		}

		virtual ResourceType GetType() const override
		{
			return GetStaticType();
		}

	private:
		entt::registry registry;
	};
};
