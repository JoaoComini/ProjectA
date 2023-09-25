#pragma once

#include <entt/entt.hpp>

#include "Entity.hpp"

#include "Rendering/Renderer.hpp"

namespace Engine
{
	class EntityManager
	{
	public:
		EntityManager() = default;

		Entity CreateEntity();
		
		template<typename... Args, typename Func>
		void ForEachEntity(Func&& func)
		{
			auto view = registry.view<Args...>();

			view.each([&](const entt::entity entity, Args&... args) {
				func(Entity(entity, &registry), args...);
			});
		}
		
	private:
		entt::registry registry;
	};
};