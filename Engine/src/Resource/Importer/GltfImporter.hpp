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
		void LoadModel(std::filesystem::path path, tinygltf::Model& model);

		void ImportTextures(std::filesystem::path parent, tinygltf::Model& model, std::vector<std::shared_ptr<Texture>>& textures);
		void ImportMaterials(std::filesystem::path parent, tinygltf::Model& model, std::vector<std::shared_ptr<Texture>>& textures, std::vector<std::shared_ptr<Material>>& materials);
		void ImportMeshes(std::filesystem::path parent, tinygltf::Model& model, std::vector<std::shared_ptr<Material>>& materials, std::vector<std::shared_ptr<Mesh>>& meshes);
		void ImportNodes(tinygltf::Model& model, std::vector<std::shared_ptr<Mesh>>& meshes, std::vector<std::unique_ptr<Node>>& nodes);

		void ImportModel(std::filesystem::path path, tinygltf::Model& gltfModel, std::vector<std::unique_ptr<Node>>& nodes);

		std::filesystem::path GetFilePath(std::filesystem::path parent, std::filesystem::path path, std::string_view suffix);

		const Vulkan::Device& device;

	};
};