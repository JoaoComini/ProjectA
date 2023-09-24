#include "Loader.hpp"

#include "EntityManager.hpp"
#include "Entity.hpp"
#include "Components.hpp"

#include <sstream>
#include <iostream>

#include <glm/ext.hpp>

namespace Scene
{
	Loader::Loader(const Vulkan::Device& device, EntityManager& entityManager) : device(device), scene(entityManager)
	{
	}

	void Loader::LoadFromGltf(std::string path)
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
        LoadMeshes(model);
        LoadNodes(model);
	}

	void Loader::LoadTextures(tinygltf::Model& model)
	{
		for (auto& texture : model.textures)
		{
			auto& image = model.images[texture.source];

			textures.push_back(std::make_shared<Rendering::Texture>(device, image.width, image.height, image.image));
		}
	}

	void Loader::LoadMaterials(tinygltf::Model& model)
	{
		for (auto& material : model.materials)
		{
			auto& values = material.values;

			Rendering::Texture* diffuse = nullptr;

			if (values.find("baseColorTexture") != values.end()) {
                auto index = values["baseColorTexture"].TextureIndex();

				diffuse = textures[index].get();
			}

			materials.emplace_back(diffuse);
		}
	}

	void Loader::LoadMeshes(tinygltf::Model& model)
	{
        for (auto& mesh : model.meshes)
        {
            for (size_t i = 0; i < mesh.primitives.size(); i++)
            {
                std::vector<Rendering::Vertex> vertices;

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

                auto mesh = std::make_shared<Rendering::Mesh>(device, materials[primitive.material], vertices);

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

                meshes.push_back(mesh);
            }
        }
	}

    void Loader::LoadNodes(tinygltf::Model& model)
    {
        for (auto& node : model.nodes)
        {
            auto entity = scene.CreateEntity();

            if (node.mesh >= 0)
            {
                auto& renderer = entity.AddComponent<Component::MeshRenderer>();
                renderer.mesh = meshes[node.mesh].get();
            }

            auto& transform = entity.AddComponent<Component::Transform>();

            if (!node.translation.empty())
            {
                std::transform(node.translation.begin(), node.translation.end(), glm::value_ptr(transform.position), [](auto value) { return static_cast<float>(value); });
            }

            if (!node.rotation.empty())
            {
                glm::quat rotation;

                std::transform(node.rotation.begin(), node.rotation.end(), glm::value_ptr(rotation), [](auto value) { return static_cast<float>(value); });

                transform.rotation = glm::eulerAngles(rotation);
            }
        }
    }
}
