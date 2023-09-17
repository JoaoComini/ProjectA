#include "Model.hpp"

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <glm/ext.hpp>

#include "Vertex.hpp"

namespace Rendering
{
	Model::Model(Vulkan::Device& device, std::string path)
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

        for (auto& texture : model.textures)
        {
            auto& image = model.images[texture.source];

            this->textures.push_back(std::make_unique<Texture>(device, image.width, image.height, image.image));
        }

        for (auto& material : model.materials)
        {
            auto& values = material.values;

            Texture* diffuse = nullptr;

            if (values.find("baseColorTexture") != values.end()) {
                diffuse = textures[values["baseColorTexture"].TextureIndex()].get();
            }

            materials.emplace_back(diffuse);
        }

        for (auto &mesh : model.meshes)
        {
            for (size_t i = 0; i < mesh.primitives.size(); i++)
            {
                std::vector<Vertex> vertices;

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
                        else if(attribute.first == "NORMAL")
                        {
                            vertex.normal = glm::make_vec3(&buffer[j * stride]);
                        }
                        else if (attribute.first == "TEXCOORD_0")
                        {
                            vertex.uv = glm::make_vec2(&buffer[j * stride]);
                        }
                    }
                }

                auto mesh = std::make_unique<Mesh>(device, materials[primitive.material], vertices);

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


                this->meshes.push_back(std::move(mesh));
            }
        }
	}

    std::vector<std::unique_ptr<Mesh>> const& Model::GetMeshes() const
    {
        return meshes;
    }
}

