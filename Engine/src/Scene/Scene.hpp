#pragma once

#include <entt/entt.hpp>

#include "Entity.hpp"

#include "Rendering/Renderer.hpp"

namespace Scene
{
	class Scene
	{
	public:
		Entity CreateEntity();
		
		void Render(Rendering::Renderer& renderer);

	private:
		entt::registry registry;
	};
};