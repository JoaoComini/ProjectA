#include "RenderBatcher.h"

#include "Resource/ResourceManager.h"
#include "Renderer.h"
#include "Scene/SceneGraph.h"

namespace Engine
{
    void RenderBatcher::BuildBatches(SceneGraph& scene, const RenderCamera& camera)
    {
		auto cameraPosition = camera.GetPosition();

		auto query = scene.Query<Component::MeshRender, Component::LocalToWorld>();

		for (const auto entity : query)
		{
			auto [meshRender, localToWorld] = query.GetComponent(entity);

			auto mesh = meshRender.mesh;

			for (auto& primitive : mesh->GetPrimitives())
			{
				auto bounds = mesh->GetBounds();
				bounds.Transform(localToWorld.value);

				auto distance = glm::length2(bounds.GetCenter() - glm::vec3{ cameraPosition });

				if (const auto material = primitive.GetMaterial(); material->GetAlphaMode() == AlphaMode::Blend)
				{
					transparents.emplace_back(localToWorld.value, &primitive, distance);
					continue;
				}

				opaques.emplace_back(localToWorld.value, &primitive, distance);
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

	void RenderBatcher::SortOpaques()
	{
		auto sort = [](const RenderGeometry& a, const RenderGeometry& b)
		{
			std::size_t hashA{ 0 };
			Hash(hashA, a.primitive->GetMaterial());

			std::size_t hashB{ 0 };
			Hash(hashB, b.primitive->GetMaterial());

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