#include "RenderSystem.hpp"

namespace Engine
{
	RenderSystem::RenderSystem(Scene& scene, Renderer& renderer) : System(scene), renderer(renderer)
	{

	}

	void RenderSystem::Update(float timestep)
	{
		auto [camera, found] = scene.FindFirstEntity<Component::Transform, Component::Camera>();

		if (!found)
		{
			return;
		}

		Component::Camera cameraComponent = camera.GetComponent<Component::Camera>();
		Component::Transform cameraTransform = camera.GetComponent<Component::Transform>();

		renderer.Begin(cameraComponent.camera, cameraTransform.GetLocalMatrix());

		scene.ForEachEntity<Component::Transform, Component::MeshRender>(
			[&](auto entity) {

				const auto& transform = entity.GetComponent<Component::Transform>();
				auto meshRender = entity.GetComponent<Component::MeshRender>();

				glm::mat4 matrix = GetWorldMatrix(entity, transform);

				renderer.Draw(*meshRender.mesh, matrix);
			}
		);

		renderer.End();
	}

	glm::mat4 RenderSystem::GetWorldMatrix(Entity entity, Component::Transform transform)
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
};