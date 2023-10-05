#pragma once

#include <entt/entt.hpp>

#include "Entity.hpp"
#include "Components.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Mesh.hpp"

#include <vector>
#include <functional>

namespace Engine
{
	class Scene
	{
	public:
		Scene();

		Entity CreateEntity();
		void DestroyEntity(Entity entity);

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
		
	private:
		void OnCreateEntity(entt::registry& registry, entt::entity entity);

		entt::registry registry;
	};
};