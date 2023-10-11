#pragma once

#include <entt/entt.hpp>

#include "Entity.hpp"
#include "Components.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Mesh.hpp"

#include <vector>
#include <unordered_map>
#include <functional>

#include "Mixin.hpp"

namespace Engine
{
	class Scene
	{
	public:
		Entity CreateEntity(Uuid id = {});
		void DestroyEntity(Entity entity);

		Entity FindEntityById(Uuid id);

		void Update();

		template<typename... Args>
		void ForEachEntity(std::function<void(Entity)> func)
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

					func({ entity, &registry });
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

					func({ entity, &registry });
				}
			}
		}

		template<typename... Args>
		std::pair<Entity, bool> FindFirstEntity(std::function<bool(Entity)> predicate = [](auto e) { return true; })
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

		void Clear()
		{
			registry.clear();
		}

	private:
		entt::registry registry;

		std::unordered_map<Uuid, Entity> entityMap;
	};
};
