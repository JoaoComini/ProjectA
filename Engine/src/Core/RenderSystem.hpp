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
		Rendering::Camera* mainCamera = nullptr;
		glm::mat4 viewMatrix;

		entityManager.ForEachEntity<Scene::Component::Transform, Scene::Component::Camera>(
			[&](const auto entity, auto& transform, auto& camera)
			{
				mainCamera = &camera.camera;
				viewMatrix = transform.GetLocalMatrix();
			}
		);

		if (mainCamera == nullptr)
		{
			return;
		}

		renderer.Begin(*mainCamera, viewMatrix);

		entityManager.ForEachEntity<Scene::Component::Transform, Scene::Component::MeshRenderer>(
			[&](auto entity, const auto& transform, const auto& mesh) {
				
				auto matrix = GetWorldMatrix(entity, transform);

				renderer.Draw(*mesh.mesh, matrix);
			}
		);

		renderer.End();
	}

	glm::mat4 GetWorldMatrix(Scene::Entity entity, Scene::Component::Transform transform)
	{
		auto relationship = entity.TryGetComponent<Scene::Component::Relationship>();

		if (relationship == nullptr)
		{
			return transform.GetLocalMatrix();
		}

		auto parentEntity = relationship->parent;
		auto parentTransform = parentEntity.TryGetComponent<Scene::Component::Transform>();

		return GetWorldMatrix(parentEntity, *parentTransform) * transform.GetLocalMatrix();
	}

private:
	Rendering::Renderer& renderer;
};
