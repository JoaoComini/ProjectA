#pragma once

#include "Rendering/Renderer.hpp"
#include "Rendering/Camera.hpp"

#include "Core/System.hpp"

#include "Scene/Scene.hpp"
#include "Scene/Components.hpp"

#include <glm/glm.hpp>


namespace Engine {

	class RenderSystem : public System
	{
	public:
		RenderSystem(Scene& scene, Renderer& renderer);

		void Update(float timestep) override;

	private:
		glm::mat4 GetWorldMatrix(Entity entity, Component::Transform transform);

		Renderer& renderer;
	};
}
