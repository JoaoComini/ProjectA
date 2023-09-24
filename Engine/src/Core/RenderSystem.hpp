#pragma once

#include "Rendering/Renderer.hpp"
#include "Rendering/Camera.hpp"

#include "Core/System.hpp"

#include "Scene/EntityManager.hpp"
#include "Scene/Components.hpp"
#include "Scene/Loader.hpp"

#include <glm/glm.hpp>

class RenderSystem : public System
{
public:
	RenderSystem(Scene::EntityManager& entityManager, Rendering::Renderer& renderer) : System(entityManager), renderer(renderer)
	{

	}

	void Update(float timestep) override
	{
		Rendering::Camera* camera = nullptr;
		glm::mat4 transform;

		entityManager.ForEachEntity<Scene::Component::Transform, Scene::Component::Camera>(
			[&](const auto entity, auto& t, auto& c)
			{
				camera = &c.camera;
				transform = t.Get();
			}
		);

		if (camera == nullptr)
		{
			return;
		}

		renderer.Begin(*camera, transform);

		entityManager.ForEachEntity<Scene::Component::Transform, Scene::Component::MeshRenderer>(
			[&](const auto entity, const auto& transform, const auto& mesh) {
				renderer.Draw(*mesh.mesh, transform.Get());
			}
		);

		renderer.End();
	}

private:
	Rendering::Renderer& renderer;
};
