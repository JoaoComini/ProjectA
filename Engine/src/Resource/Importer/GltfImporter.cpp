#include "GltfImporter.hpp"

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Resource/Factory/TextureFactory.hpp"
#include "Resource/Factory/MaterialFactory.hpp"
#include "Resource/Factory/MeshFactory.hpp"

#include "Resource/ResourceManager.hpp"

#include "Project/Project.hpp"

namespace Engine
{
    GltfImporter::GltfImporter(Vulkan::Device& device)
        : device(device) 
    {
    }

    void GltfImporter::Import(std::filesystem::path path)
    {   
        tinygltf::Model model = LoadModel(path);

        auto directory = GetPrefabDirectory(path);

        if (!std::filesystem::create_directory(directory))
        {
            return;
        }

        auto scene = Scene{};
    
        auto textures = ImportTextures(directory, model);

        auto materials = ImportMaterials(directory, model, textures);

        auto meshes = ImportMeshes(directory, model, materials);

        auto entities =  ImportEntities(model, meshes, scene);
        
        SetupRelationship(directory.stem().string(), model, scene, entities);

        ResourceManager::Get().CreateResource<Scene>(directory.stem() / directory.stem(), scene);
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

            auto id = ResourceManager::Get().CreateResource<Texture>(parent.stem() / "texture", *texture);

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

            auto alphaModeIt = alphaModes.find(material.alphaMode);

            if (alphaModeIt != alphaModes.end())
            {
                spec.alphaMode = alphaModeIt->second;
            }

            if (spec.alphaMode == AlphaMode::Mask)
            {
                spec.alphaCutoff = material.alphaCutoff;
            }

            auto id = ResourceManager::Get().CreateResource<Material>(parent.stem() / "material", spec);

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

                primitiveSpec.vertices = vertices;

                std::vector<uint8_t> indices;
                auto indexType = VK_INDEX_TYPE_MAX_ENUM;

                if (primitive.indices >= 0)
                {
                    const auto& accessor = model.accessors[primitive.indices];
                    const auto& view = model.bufferViews[accessor.bufferView];
                    auto buffer = model.buffers[view.buffer];

                    void* data = &(buffer.data[accessor.byteOffset + view.byteOffset]);

                    auto count = accessor.count;
                    auto size = accessor.ByteStride(view) * count;

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

                    indices.insert(indices.end(), static_cast<uint8_t*>(data), static_cast<uint8_t*>(data) + size);

                    primitiveSpec.indices = indices;
                    primitiveSpec.indexType = indexType;
                }

                if (materials[primitive.material])
                {
                    primitiveSpec.material = materials[primitive.material];
                }

                spec.primitives.push_back(primitiveSpec);
            }

            auto id = ResourceManager::Get().CreateResource<Mesh>(parent.stem() / mesh.name, spec);

            meshes.push_back(id);
        }

        return meshes;
    }

    std::vector<Entity> GltfImporter::ImportEntities(tinygltf::Model& gltfModel, std::vector<ResourceId>& meshes, Scene& scene)
    {
        std::vector<Entity> entities;

        for (auto& gltfNode : gltfModel.nodes)
        {
            auto entity = scene.CreateEntity();

            if (gltfNode.mesh >= 0)
            {
                entity.AddComponent<Component::MeshRender>(meshes[gltfNode.mesh]);
            }

            auto& transform = entity.GetComponent<Component::Transform>();

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
                entity.SetName(gltfNode.name);
            }

            entities.push_back(entity);
        }

        return entities;
    }

    void GltfImporter::SetupRelationship(const std::string& name, tinygltf::Model& gltfModel, Scene& scene, std::vector<Entity>& entities)
    {
        auto root = scene.CreateEntity();
        root.SetName(name);

        for (auto& gltfScene : gltfModel.scenes)
        {
            std::queue<std::pair<Entity, int>> traverseNodes;

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

                current.SetParent(parent);

                for (auto index: gltfModel.nodes[it.second].children)
                {
                    traverseNodes.push(std::make_pair(current, index));
                }
            }
        }
    }

    std::filesystem::path GltfImporter::GetPrefabDirectory(std::filesystem::path path)
    {
        auto base = Project::GetResourceDirectory() / path.stem();

        std::filesystem::path directory;
        auto current = 0;
        do
        {
            directory = base;

            if (current > 0)
            {
                directory += "_" + std::to_string(current);
            }

            current += 1;
        } while (std::filesystem::exists(directory));

        return directory;
    }
};