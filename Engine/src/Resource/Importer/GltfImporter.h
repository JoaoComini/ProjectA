#pragma once

#include "Scene/Scene.h"

#include "Resource/ResourceImporter.h"

namespace tinygltf
{
	class Model;
}

namespace Engine
{
	class GltfImporter final : public ResourceImporter
	{
	public:
		void Import(const std::filesystem::path& source, const std::filesystem::path& destination) override;
		[[nodiscard]] std::vector<std::string> GetImportExtensions() const override;
	private:
		tinygltf::Model LoadModel(const std::filesystem::path& path);

		std::vector<std::shared_ptr<Texture>> ImportTextures(const tinygltf::Model& model);
		std::vector<std::shared_ptr<Material>> ImportMaterials(const tinygltf::Model& model, const std::vector<std::shared_ptr<Texture>>& textures);
		std::vector<std::shared_ptr<Mesh>> ImportMeshes(const tinygltf::Model& model, const std::vector<std::shared_ptr<Material>>& materials);
		std::vector<Entity::Id> ImportEntities(tinygltf::Model& model, std::vector<std::shared_ptr<Mesh>>& meshes, Scene &scene);

		void SetupRelationship(const std::string& name, const tinygltf::Model& gltfModel, Scene& scene, std::vector<Entity::Id>& entities);
	};
};