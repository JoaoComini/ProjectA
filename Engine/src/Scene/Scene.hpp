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

		Scene& operator=(const Scene& other)
		{
			Resource::operator=(other);

			registry.clear();
			entityMap.clear();

			const auto entities = other.registry.storage<entt::entity>();
			registry.storage<entt::entity>().push(entities->cbegin(), entities->cend());

			CopyComponent<Component::Transform>(other.registry);
			CopyComponent<Component::Id>(other.registry);
			CopyComponent<Component::Name>(other.registry);
			CopyComponent<Component::MeshRender>(other.registry);
			CopyComponent<Component::Camera>(other.registry);

			CopyRelationships(other.registry);

			ForEachEntity<Component::Id>([&](Entity entity) {
				auto id = entity.GetComponent<Component::Id>().id;
				entityMap[id] = entity;
			});

			return *this;
		}

		Entity CreateEntity(Uuid id = {});
		void DestroyEntity(Entity entity);

		Entity FindEntityById(Uuid id);

		void Update();

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

					func(Entity{ entity, &registry });
				}
			}
			else
			{
				auto view = registry.view<Args...>();

				for (auto entity : view)
				{
					if (!registry.valid(entity))
					{
						continue;
					}

					func(Entity{ entity, &registry });
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

		static ResourceType GetStaticType()
		{
			return ResourceType::Scene;
		}

		virtual ResourceType GetType() const override
		{
			return GetStaticType();
		}

	private:

		template<typename T>
		void CopyComponent(const entt::registry& other)
		{
			const auto components = other.storage<T>();

			if (components)
			{
				registry.insert<T>(components->entt::sparse_set::begin(), components->entt::sparse_set::end(), components->begin());
			}
		}

		void CopyRelationships(const entt::registry& other)
		{
			const auto components = other.storage<Component::Relationship>();

			if (components)
			{
				std::vector<Component::Relationship> transformed;

				for (auto it = components->begin(); it != components->end(); it++)
				{
					transformed.push_back({
						.children = it->children,
						.first = { it->first, &registry },
						.prev = { it->prev, &registry },
						.next = { it->next, &registry },
						.parent = { it->parent, &registry }
					});
				}

				registry.insert<Component::Relationship>(components->entt::sparse_set::begin(), components->entt::sparse_set::end(), transformed.begin());
			}
		}

		entt::registry registry;

		std::unordered_map<Uuid, Entity> entityMap;
	};
};
