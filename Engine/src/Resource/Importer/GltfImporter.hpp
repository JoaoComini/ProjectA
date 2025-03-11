#pragma once

#include "Scene/Scene.hpp"

#include "Resource/ResourceImporter.hpp"

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
		std::vector<std::string> GetSupportedExtensions() const override;
	private:
		tinygltf::Model LoadModel(std::filesystem::path path);

		std::vector<ResourceId> ImportTextures(std::filesystem::path parent, tinygltf::Model& model);
		std::vector<ResourceId> ImportMaterials(std::filesystem::path parent, tinygltf::Model& model, std::vector<ResourceId>& textures);
		std::vector<ResourceId> ImportMeshes(std::filesystem::path parent, tinygltf::Model& model, std::vector<ResourceId>& materials);
		std::vector<Entity::Id> ImportEntities(tinygltf::Model& model, std::vector<ResourceId>& meshes, Scene &scene);

		void SetupRelationship(const std::string& name, tinygltf::Model& gltfModel, Scene& scene, std::vector<Entity::Id>& entities);
	};
};