#pragma once

#include <tiny_gltf.h>

#include "EntityManager.hpp"

#include "Rendering/Texture.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Material.hpp"

#include "Vulkan/Device.hpp"

namespace Scene
{
	class Loader
	{
	public:
		Loader(const Vulkan::Device& device, EntityManager& entityManager);


		void LoadFromGltf(std::string path);

	private:
		void LoadTextures(tinygltf::Model& model);
		void LoadMaterials(tinygltf::Model& model);
		void LoadMeshes(tinygltf::Model& model);
		void LoadNodes(tinygltf::Model& model);

		const Vulkan::Device& device;
		EntityManager& scene;

		std::vector<std::shared_ptr<Rendering::Texture>> textures;
		std::vector<std::shared_ptr<Rendering::Mesh>> meshes;
		std::vector<Rendering::Material> materials;
	};
}