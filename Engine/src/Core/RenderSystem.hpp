#pragma once

#include "Rendering/Renderer.hpp"
#include "Rendering/Gui.hpp"

#include "Core/System.hpp"

#include "Scene/Scene.hpp"
#include "Scene/Components.hpp"

#include <glm/glm.hpp>

namespace Engine {

	class RenderSystem : public System
	{
	public:
		RenderSystem(Scene& scene);

		void Update(float timestep) override;

	private:
		Gui* gui;
		Renderer* renderer;

		glm::mat4 GetWorldMatrix(Entity entity, Component::Transform transform);
	};
}
