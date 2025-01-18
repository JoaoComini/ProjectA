#include "RenderBatcher.hpp"

#include "Resource/ResourceManager.hpp"
#include "Renderer.hpp"
#include "Scene/Scene.hpp"

namespace Engine
{
	RenderBatcher::RenderBatcher(Scene& scene) : scene(scene) { }

    void RenderBatcher::BuildBatches()
    {
		auto [_, cameraTransform] = Renderer::Get().GetMainCamera();
		auto cameraPosition = cameraTransform[3];

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

	void RenderBatcher::Reset()
	{
		opaques.clear();
		transparents.clear();
	}

	Material& RenderBatcher::GetMaterial(Primitive& primitive)
	{
		return *ResourceManager::Get().LoadResource<Material>(primitive.GetMaterial());
	}

	void RenderBatcher::SortOpaques()
	{
		auto sort = [](const RenderGeometry& a, const RenderGeometry& b)
		{
			std::size_t hasha{ 0 };
			Hash(hasha, a.material);

			std::size_t hashb{ 0 };
			Hash(hashb, b.material);

			return hasha < hashb;
		};

		std::sort(opaques.begin(), opaques.end(), sort);
	}

	void RenderBatcher::SortTransparents()
	{
		auto sort = [](const RenderGeometry& a, const RenderGeometry& b)
		{
			return a.distance < b.distance;
		};

		std::sort(transparents.begin(), transparents.end(), sort);
	}
}