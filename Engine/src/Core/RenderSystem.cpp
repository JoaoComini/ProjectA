#include "RenderSystem.hpp"

#include "Resource/ResourceManager.hpp"

namespace Engine
{
	RenderSystem::RenderSystem(Scene& scene) : System(scene)
	{
		renderer = &Renderer::Get();
		gui = &Gui::Get();
	}

	void RenderSystem::Update(float timestep)
	{
		scene.ForEachEntity<Component::Transform, Component::MeshRender>(
			[&](auto entity) {

				const auto& transform = entity.GetComponent<Component::Transform>();
				auto meshRender = entity.GetComponent<Component::MeshRender>();

				if (!meshRender.mesh)
				{
					return;
				}

				auto mesh = ResourceManager::Get().LoadResource<Mesh>(meshRender.mesh);

				glm::mat4 matrix = GetWorldMatrix(entity, transform);

				renderer->Draw(*mesh, matrix);
			}
		);
	}

	glm::mat4 RenderSystem::GetWorldMatrix(Entity entity, Component::Transform transform)
	{
		auto parent = entity.GetParent();

		if (!parent)
		{
			return transform.GetLocalMatrix();
		}

		auto parentTransform = parent.TryGetComponent<Component::Transform>();

		if (!parentTransform)
		{
			return transform.GetLocalMatrix();
		}

		return GetWorldMatrix(parent, *parentTransform) * transform.GetLocalMatrix();
	}
};