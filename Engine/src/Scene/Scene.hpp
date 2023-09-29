#pragma once

#include <entt/entt.hpp>

#include "Entity.hpp"

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
		Scene() = default;

		Entity CreateEntity();
		
		template<typename... Args>
		void ForEachEntity(std::function<void(Entity)> func)
		{
			auto view = registry.view<Args...>();

			for (auto entity : view) {
				func({ entity, &registry });
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
		entt::registry registry;

		std::vector<std::unique_ptr<Engine::Texture>> textures;
		std::vector<std::unique_ptr<Engine::Mesh>> meshes;
	};
};