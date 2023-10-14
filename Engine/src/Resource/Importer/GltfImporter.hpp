#pragma once

#include <tiny_gltf.h>

#include <filesystem>

#include "Vulkan/Device.hpp"

#include "Rendering/Texture.hpp"
#include "Rendering/Mesh.hpp"

#include "Resource/Model.hpp"

namespace Engine
{
	class GltfImporter
	{
	public:
		GltfImporter(const Vulkan::Device& device);

		void Import(std::filesystem::path path);

	private:
		tinygltf::Model LoadModel(std::filesystem::path path);

		std::vector<ResourceId> ImportTextures(std::filesystem::path parent, tinygltf::Model& model);
		std::vector<ResourceId> ImportMaterials(std::filesystem::path parent, tinygltf::Model& model, std::vector<ResourceId>& textures);
		std::vector<ResourceId> ImportMeshes(std::filesystem::path parent, tinygltf::Model& model, std::vector<ResourceId>& materials);
		std::vector<std::unique_ptr<Node>> ImportNodes(tinygltf::Model& model, std::vector<ResourceId>& meshes);

		void ImportModel(std::filesystem::path path, tinygltf::Model& gltfModel, std::vector<std::unique_ptr<Node>>& nodes);

		std::filesystem::path GetFilePath(std::filesystem::path parent, std::string name, std::string extension);

		const Vulkan::Device& device;

	};
};