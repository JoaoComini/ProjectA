#pragma once

#include <tiny_gltf.h>

#include "Scene.hpp"

#include "Rendering/Texture.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Material.hpp"

#include "Vulkan/Device.hpp"

namespace Scene
{
	class Loader
	{
	public:
		Loader(const Vulkan::Device& device, Scene& scene);


		void LoadFromGltf(std::string path);

	private:
		void LoadTextures(tinygltf::Model& model);
		void LoadMaterials(tinygltf::Model& model);
		void LoadMeshes(tinygltf::Model& model);

		const Vulkan::Device& device;
		Scene& scene;

		std::vector<std::shared_ptr<Rendering::Texture>> textures;
		std::vector<std::shared_ptr<Rendering::Mesh>> meshes;
		std::vector<Rendering::Material> materials;
	};
}