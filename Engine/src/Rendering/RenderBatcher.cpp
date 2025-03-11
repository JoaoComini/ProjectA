#include "RenderBatcher.hpp"

#include "Resource/ResourceManager.hpp"
#include "Renderer.hpp"
#include "Scene/Scene.hpp"

namespace Engine
{
    void RenderBatcher::BuildBatches(Scene& scene, const RenderCamera& camera)
    {
		auto cameraPosition = camera.GetPosition();

		auto query = scene.Query<Component::MeshRender, Component::LocalToWorld>();

		for (auto entity : query)
		{
			auto [meshRender, localToWorld] = query.GetComponent(entity);

			auto mesh = ResourceManager::Get().LoadResource<Mesh>(meshRender.mesh);

			for (auto& primitive : mesh->GetPrimitives())
			{
				auto& material = GetMaterial(*primitive);

				auto bounds = mesh->GetBounds();
				bounds.Transform(localToWorld.value);

				auto distance = glm::length2(bounds.GetCenter() - glm::vec3{ cameraPosition });

				if (material.GetAlphaMode() == AlphaMode::Blend)
				{
					transparents.emplace_back(localToWorld.value, primitive.get(), &material, distance);
					continue;
				}

				opaques.emplace_back(localToWorld.value, primitive.get(), &material, distance);
			}
		}

		SortOpaques();
		SortTransparents();
    }

	const std::vector<RenderGeometry>& RenderBatcher::GetOpaques()
	{
		return opaques;
	}

	const std::vector<RenderGeometry>& RenderBatcher::GetTransparents()
	{
		return transparents;
	}

	Material& RenderBatcher::GetMaterial(const Primitive& primitive)
	{
		return *ResourceManager::Get().LoadResource<Material>(primitive.GetMaterial());
	}

	void RenderBatcher::SortOpaques()
	{
		auto sort = [](const RenderGeometry& a, const RenderGeometry& b)
		{
			std::size_t hashA{ 0 };
			Hash(hashA, a.material);

			std::size_t hashB{ 0 };
			Hash(hashB, b.material);

			return hashA < hashB;
		};

		std::ranges::sort(opaques, sort);
	}

	void RenderBatcher::SortTransparents()
	{
		auto sort = [](const RenderGeometry& a, const RenderGeometry& b)
		{
			return a.distance > b.distance;
		};

		std::ranges::sort(transparents, sort);
	}
}