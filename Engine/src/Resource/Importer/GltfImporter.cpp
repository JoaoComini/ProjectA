#include "GltfImporter.hpp"

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Resource/Factory/TextureFactory.hpp"
#include "Resource/Factory/MaterialFactory.hpp"
#include "Resource/Factory/MeshFactory.hpp"

#include "Resource/ResourceManager.hpp"

namespace Engine
{
    void GltfImporter::Import(const std::filesystem::path &source, const std::filesystem::path &destination)
    {
        tinygltf::Model model = LoadModel(source);

        auto scene = Scene{};

        auto textures = ImportTextures(destination, model);

        auto materials = ImportMaterials(destination, model, textures);

        auto meshes = ImportMeshes(destination, model, materials);

        auto entities =  ImportEntities(model, meshes, scene);

        SetupRelationship(destination.stem().string(), model, scene, entities);

        ResourceManager::Get().CreateResource<Scene>(destination.stem(), scene);
    }

    std::vector<std::string> GltfImporter::GetSupportedExtensions() const
    {
        return { ".glb" };
    }

    tinygltf::Model GltfImporter::LoadModel(std::filesystem::path path)
    {
        tinygltf::TinyGLTF loader;

        tinygltf::Model model;
        std::string err;
        std::string warn;

        loader.LoadBinaryFromFile(&model, &err, &warn, path.string());

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

        return model;
    }

    std::vector<ResourceId> GltfImporter::ImportTextures(std::filesystem::path parent, tinygltf::Model& model)
    {
        std::vector<ResourceId> textures;

        for (size_t i = 0; i < model.textures.size(); i++)
        {
            auto& gltfTexture = model.textures[i];
            auto& image = model.images[gltfTexture.source];

            std::vector<Mipmap> mipmaps
            {
                Mipmap{
                    0,
                    0,
                    {
                        static_cast<uint32_t>(image.width),
                        static_cast<uint32_t>(image.height),
                        1
                    }
                }
            };

            std::vector<uint8_t> data = image.image;

            auto texture = std::make_shared<Texture>(std::move(data), std::move(mipmaps));
            texture->GenerateMipmaps();

            auto id = ResourceManager::Get().CreateResource<Texture>(parent.stem().string() + "_" + std::to_string(i), *texture);

            textures.push_back(id);
        }

        return textures;
    }

    std::vector<ResourceId> GltfImporter::ImportMaterials(std::filesystem::path parent, tinygltf::Model& model, std::vector<ResourceId>& textures)
    {
        std::vector<ResourceId> materials;

        for (size_t i = 0; i < model.materials.size(); i++)
        {
            auto& material = model.materials[i];
            auto& pbr = material.pbrMetallicRoughness;
            MaterialSpec spec{};

            if (pbr.baseColorTexture.index != -1) {
                spec.albedoTexture = textures[pbr.baseColorTexture.index];
            }

            if (pbr.metallicRoughnessTexture.index != -1)
            {
                spec.metallicRoughnessTexture = textures[pbr.metallicRoughnessTexture.index];
            }

            if (material.normalTexture.index != -1)
            {
                spec.normalTexture = textures[material.normalTexture.index];
            }

            spec.albedoColor = glm::make_vec4(pbr.baseColorFactor.data());
            spec.metallicFactor = pbr.metallicFactor;
            spec.roughnessFactor = pbr.roughnessFactor;

            std::unordered_map<std::string, AlphaMode> alphaModes{
                { "OPAQUE", AlphaMode::Opaque },
                { "BLEND", AlphaMode::Blend },
                { "MASK", AlphaMode::Mask }
            };

            if (auto it = alphaModes.find(material.alphaMode); it != alphaModes.end())
            {
                spec.alphaMode = it->second;
            }

            if (spec.alphaMode == AlphaMode::Mask)
            {
                spec.alphaCutoff = material.alphaCutoff;
            }

            auto id = ResourceManager::Get().CreateResource<Material>(parent.stem().string() + "_" + std::to_string(i), spec);

            materials.push_back(id);
        }

        return materials;
    }

    std::vector<ResourceId> GltfImporter::ImportMeshes(std::filesystem::path parent, tinygltf::Model& model, std::vector<ResourceId>& materials)
    {
        std::vector<ResourceId> meshes;

        for (auto& mesh : model.meshes)
        {
            MeshSpec spec{};

            for (size_t i = 0; i < mesh.primitives.size(); i++)
            {
                PrimitiveSpec primitiveSpec{};

                std::vector<Engine::Vertex> vertices;

                const auto& primitive = mesh.primitives[i];

                const auto& position = primitive.attributes.find("POSITION");

                vertices.resize(model.accessors[position->second].count);

                for (auto& attribute : primitive.attributes)
                {
                    const auto& accessor = model.accessors[attribute.second];
                    const auto& view = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[view.buffer];

                    auto stride = accessor.ByteStride(view) / sizeof(float);
                    auto data = reinterpret_cast<const float*>(buffer.data.data() + accessor.byteOffset + view.byteOffset);
                    auto count = static_cast<uint32_t>(accessor.count);

                    for (size_t j = 0; j < count; j++)
                    {
                        auto& vertex = vertices[j];

                        if (attribute.first == "POSITION")
                        {
                            vertex.position = glm::make_vec3(&data[j * stride]);
                        }
                        else if (attribute.first == "NORMAL")
                        {
                            vertex.normal = glm::make_vec3(&data[j * stride]);
                        }
                        else if (attribute.first == "TEXCOORD_0")
                        {
                            vertex.uv = glm::make_vec2(&data[j * stride]);
                        }
                    }
                }

                primitiveSpec.vertices = vertices;

                std::vector<uint8_t> indices;
                auto indexType = VK_INDEX_TYPE_MAX_ENUM;

                if (primitive.indices >= 0)
                {
                    const auto& accessor = model.accessors[primitive.indices];
                    const auto& view = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[view.buffer];

                    const uint8_t* data = buffer.data.data() + accessor.byteOffset + view.byteOffset;

                    auto size = accessor.ByteStride(view) * accessor.count;

                    switch (accessor.componentType)
                    {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        indexType = VK_INDEX_TYPE_UINT8_KHR;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        indexType = VK_INDEX_TYPE_UINT16;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        indexType = VK_INDEX_TYPE_UINT32;
                        break;
                    }

                    indices.insert(indices.end(), data, data + size);

                    primitiveSpec.indices = indices;
                    primitiveSpec.indexType = indexType;
                }

                if (materials[primitive.material])
                {
                    primitiveSpec.material = materials[primitive.material];
                }

                spec.primitives.push_back(primitiveSpec);
            }

            auto id = ResourceManager::Get().CreateResource<Mesh>(parent.stem().string() + "_" + mesh.name, spec);

            meshes.push_back(id);
        }

        return meshes;
    }

    std::vector<Entity::Id> GltfImporter::ImportEntities(tinygltf::Model& gltfModel, std::vector<ResourceId>& meshes, Scene& scene)
    {
        std::vector<Entity::Id> entities;

        for (auto& gltfNode : gltfModel.nodes)
        {
            auto entity = scene.CreateEntity();

            if (gltfNode.mesh >= 0)
            {
                scene.AddComponent<Component::MeshRender>(entity, meshes[gltfNode.mesh]);
            }

            auto& transform = scene.GetComponent<Component::Transform>(entity);

            if (!gltfNode.translation.empty())
            {
                std::transform(gltfNode.translation.begin(), gltfNode.translation.end(), glm::value_ptr(transform.position), [](auto value) { return static_cast<float>(value); });
            }

            if (!gltfNode.rotation.empty())
            {
                std::transform(gltfNode.rotation.begin(), gltfNode.rotation.end(), glm::value_ptr(transform.rotation), [](auto value) { return static_cast<float>(value); });
            }

            if (!gltfNode.name.empty())
            {
                scene.GetComponent<Component::Name>(entity).name = gltfNode.name;
            }

            entities.push_back(entity);
        }

        return entities;
    }

    void GltfImporter::SetupRelationship(const std::string& name, tinygltf::Model& gltfModel, Scene& scene, std::vector<Entity::Id>& entities)
    {
        auto root = scene.CreateEntity();
        scene.GetComponent<Component::Name>(root).name = name;

        for (auto& gltfScene : gltfModel.scenes)
        {
            std::queue<std::pair<Entity::Id, int>> traverseNodes;

            for (auto index : gltfScene.nodes)
            {
                traverseNodes.push(std::make_pair(root, index));
            }

            while (! traverseNodes.empty())
            {
                auto& it = traverseNodes.front();
                traverseNodes.pop();

                auto& current = entities[it.second];
                auto& parent = it.first;

                scene.SetParent(current, parent);

                for (auto index: gltfModel.nodes[it.second].children)
                {
                    traverseNodes.push(std::make_pair(current, index));
                }
            }
        }
    }
};