#pragma once

#include "Rendering/Renderer.hpp"
#include "Rendering/Camera.hpp"

#include "Core/System.hpp"

#include "Scene/EntityManager.hpp"
#include "Scene/Components.hpp"
#include "Scene/Loader.hpp"

#include <glm/glm.hpp>


namespace Engine {

	class RenderSystem : public System
	{
	public:
		RenderSystem(EntityManager& entityManager, Renderer& renderer) : System(entityManager), renderer(renderer)
		{

		}

		void Update(float timestep) override
		{
			Camera* mainCamera = nullptr;
			glm::mat4 viewMatrix;

			entityManager.ForEachEntity<Component::Transform, Component::Camera>(
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

			entityManager.ForEachEntity<Component::Transform, Component::MeshRenderer>(
				[&](auto entity, const auto& transform, const auto& mesh) {

					auto matrix = GetWorldMatrix(entity, transform);

					renderer.Draw(*mesh.mesh, matrix);
				}
			);

			renderer.End();
		}

		glm::mat4 GetWorldMatrix(Entity entity, Component::Transform transform)
		{
			auto relationship = entity.TryGetComponent<Component::Relationship>();

			if (relationship == nullptr)
			{
				return transform.GetLocalMatrix();
			}

			auto parentEntity = relationship->parent;
			auto parentTransform = parentEntity.TryGetComponent<Component::Transform>();

			return GetWorldMatrix(parentEntity, *parentTransform) * transform.GetLocalMatrix();
		}

	private:
		Renderer& renderer;
	};
}
