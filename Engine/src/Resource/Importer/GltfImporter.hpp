#pragma once

#include <tiny_gltf.h>

#include "Vulkan/Device.hpp"

#include "Rendering/Texture.hpp"
#include "Rendering/Mesh.hpp"

#include "Scene/Scene.hpp"

namespace Engine
{
	class GltfImporter
	{
	public:
		GltfImporter(Vulkan::Device& device);

		void Import(std::filesystem::path path);

	private:
		tinygltf::Model LoadModel(std::filesystem::path path);

		std::vector<ResourceId> ImportTextures(std::filesystem::path parent, tinygltf::Model& model);
		std::vector<ResourceId> ImportMaterials(std::filesystem::path parent, tinygltf::Model& model, std::vector<ResourceId>& textures);
		std::vector<ResourceId> ImportMeshes(std::filesystem::path parent, tinygltf::Model& model, std::vector<ResourceId>& materials);
		std::vector<Entity> ImportEntities(tinygltf::Model& model, std::vector<ResourceId>& meshes, Scene &scene);

		void SetupRelationship(const std::string& name, tinygltf::Model& gltfModel, Scene& scene, std::vector<Entity>& entities);

		std::filesystem::path GetPrefabDirectory(std::filesystem::path path);

		Vulkan::Device& device;

	};
};