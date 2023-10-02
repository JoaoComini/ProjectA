#include "SceneLoader.hpp"

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Scene.hpp"
#include "Entity.hpp"
#include "Components.hpp"

#include <sstream>
#include <iostream>
#include <queue>

#include <glm/ext.hpp>

namespace Engine
{
	SceneLoader::SceneLoader(const Vulkan::Device& device, Scene& scene) : device(device), scene(scene)
	{
	}

	void SceneLoader::LoadFromGltf(std::string path)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;

		std::string err;
		std::string warn;

		loader.LoadBinaryFromFile(&model, &err, &warn, path.c_str());

		if (!warn.empty())
		{
			std::cout << "WARN: " << warn << std::endl;
		}

		if (!err.empty())
		{
			std::stringstream ss;
			ss << "failed to load " << path << ": " << err;
			throw std::runtime_error(ss.str());
		}

		LoadTextures(model);
		LoadMaterials(model);
        LoadMeshes(model, path);
        LoadNodes(model);
        LoadRelationships(model);
	}

	void SceneLoader::LoadTextures(tinygltf::Model& model)
	{
		for (auto& texture : model.textures)
		{
			auto& image = model.images[texture.source];

			textures.push_back(std::make_shared<Engine::Texture>(device, image.width, image.height, image.image));
		}
	}

	void SceneLoader::LoadMaterials(tinygltf::Model& model)
	{
		for (auto& material : model.materials)
		{
			auto& values = material.values;

			std::shared_ptr<Engine::Texture> diffuse = nullptr;

			if (values.find("baseColorTexture") != values.end()) {
                auto index = values["baseColorTexture"].TextureIndex();

                diffuse = textures[index];
			}

			materials.emplace_back(diffuse);
		}
	}

	void SceneLoader::LoadMeshes(tinygltf::Model& model, std::string path)
	{
        for (auto& mesh : model.meshes)
        {
            for (size_t i = 0; i < mesh.primitives.size(); i++)
            {
                std::vector<Engine::Vertex> vertices;

                const auto& primitive = mesh.primitives[i];

                const auto& position = primitive.attributes.find("POSITION");

                assert(position != primitive.attributes.end());

                vertices.resize(model.accessors[position->second].count);

                for (auto& attribute : primitive.attributes)
                {
                    const auto& accessor = model.accessors[attribute.second];
                    const auto& view = model.bufferViews[accessor.bufferView];

                    auto buffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    auto count = static_cast<uint32_t>(accessor.count);
                    auto stride = accessor.ByteStride(view) / sizeof(float);

                    for (size_t j = 0; j < vertices.size(); j++)
                    {
                        auto& vertex = vertices[j];

                        if (attribute.first == "POSITION")
                        {
                            vertex.position = glm::make_vec3(&buffer[j * stride]);
                        }
                        else if (attribute.first == "NORMAL")
                        {
                            vertex.normal = glm::make_vec3(&buffer[j * stride]);
                        }
                        else if (attribute.first == "TEXCOORD_0")
                        {
                            vertex.uv = glm::make_vec2(&buffer[j * stride]);
                        }
                    }
                }

                auto mesh = std::make_shared<Engine::Mesh>(device, materials[primitive.material], vertices, path);

                if (primitive.indices >= 0)
                {
                    const auto& accessor = model.accessors[primitive.indices];
                    const auto& view = model.bufferViews[accessor.bufferView];
                    auto buffer = model.buffers[view.buffer];

                    void* data = &(buffer.data[accessor.byteOffset + view.byteOffset]);

                    auto count = accessor.count;
                    auto size = accessor.ByteStride(view) * count;

                    auto type = VK_INDEX_TYPE_MAX_ENUM;

                    switch (accessor.componentType)
                    {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        type = VK_INDEX_TYPE_UINT16;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        type = VK_INDEX_TYPE_UINT32;
                        break;
                    }

                    mesh->BuildIndexBuffer(data, size, count, type);
                }

                meshes.push_back(std::move(mesh));
            }
        }
	}

    void SceneLoader::LoadNodes(tinygltf::Model& model)
    {
        for (auto& node : model.nodes)
        {
            auto entity = scene.CreateEntity();

            if (node.mesh >= 0)
            {
                auto& renderer = entity.AddComponent<Component::MeshRender>();
                renderer.mesh = meshes[node.mesh];
            }

            auto& transform = entity.AddComponent<Component::Transform>();

            if (!node.translation.empty())
            {
                std::transform(node.translation.begin(), node.translation.end(), glm::value_ptr(transform.position), [](auto value) { return static_cast<float>(value); });
            }

            if (!node.rotation.empty())
            {
                std::transform(node.rotation.begin(), node.rotation.end(), glm::value_ptr(transform.rotation), [](auto value) { return static_cast<float>(value); });
            }

            auto& name = entity.GetComponent<Component::Name>();

            if (!node.name.empty())
            {
                name.name = node.name;
            }

            entities.push_back(entity);
        }
    }

    void SceneLoader::LoadRelationships(tinygltf::Model& model)
    {
        for (size_t i = 0; i < model.nodes.size(); i++)
        {
            auto &node = model.nodes[i];

            auto parent = entities[i];

            for (int j = 0; j < node.children.size(); j++)
            {
                int index = node.children[j];

                auto curr = entities[index];

                curr.SetParent(parent);
            }
        }
    }
}
