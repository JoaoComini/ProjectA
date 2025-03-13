#include "GltfModule.h"

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace Engine
{
    std::unique_ptr<Scene> GltfModule::Import(const std::filesystem::path &source)
    {
        tinygltf::Model model = LoadModel(source);

        auto scene = std::make_unique<Scene>();

        auto textures = ImportTextures(model);

        auto materials = ImportMaterials(model, textures);

        auto meshes = ImportMeshes(model, materials);

        auto entities =  ImportEntities(model, meshes, *scene);

        SetupRelationship(source.stem().string(), model, *scene, entities);

        return scene;
    }

    std::vector<std::string> GltfModule::GetImportExtensions() const
    {
        return { ".glb" };
    }

    tinygltf::Model GltfModule::LoadModel(const std::filesystem::path& path)
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

    std::vector<std::shared_ptr<Texture>> GltfModule::ImportTextures(const tinygltf::Model& model)
    {
        std::vector<std::shared_ptr<Texture>> textures;

        for (size_t i = 0; i < model.textures.size(); i++)
        {
            auto& gltfTexture = model.textures[i];
            auto& image = model.images[gltfTexture.source];

            std::vector mipmaps
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

            textures.push_back(texture);
        }

        return textures;
    }

    std::vector<std::shared_ptr<Material>> GltfModule::ImportMaterials(const tinygltf::Model& model, const std::vector<std::shared_ptr<Texture>>& textures)
    {
        std::vector<std::shared_ptr<Material>> materials;

        for (const auto& gltfMaterial : model.materials)
        {
            auto& pbr = gltfMaterial.pbrMetallicRoughness;

            std::shared_ptr<Texture> albedo;
            std::shared_ptr<Texture> normal;
            std::shared_ptr<Texture> metallicRoughness;

            if (pbr.baseColorTexture.index != -1) {
                albedo = textures[pbr.baseColorTexture.index];
            }

            if (pbr.metallicRoughnessTexture.index != -1)
            {
                metallicRoughness = textures[pbr.metallicRoughnessTexture.index];
            }

            if (gltfMaterial.normalTexture.index != -1)
            {
                normal = textures[gltfMaterial.normalTexture.index];
            }


            std::unordered_map<std::string, AlphaMode> alphaModes {
                { "OPAQUE", AlphaMode::Opaque },
                { "BLEND", AlphaMode::Blend },
                { "MASK", AlphaMode::Mask }
            };

            auto alphaMode = AlphaMode::Opaque;

            if (const auto it = alphaModes.find(gltfMaterial.alphaMode); it != alphaModes.end())
            {
                alphaMode = it->second;
            }

            auto material = std::make_shared<Material>(
                albedo,
                normal,
                metallicRoughness,
                glm::make_vec4(pbr.baseColorFactor.data()),
                static_cast<float>(pbr.metallicFactor),
                static_cast<float>(pbr.roughnessFactor),
                alphaMode,
                gltfMaterial.alphaCutoff
            );

            materials.push_back(material);
        }

        return materials;
    }

    std::vector<std::shared_ptr<Mesh>> GltfModule::ImportMeshes(const tinygltf::Model& model, const std::vector<std::shared_ptr<Material>>& materials)
    {
        std::vector<std::shared_ptr<Mesh>> meshes;

        for (const auto& gltfMesh : model.meshes)
        {
            auto mesh = std::make_shared<Mesh>();

            for (const auto& gltfPrimitive : gltfMesh.primitives)
            {
                auto primitive = std::make_unique<Primitive>();

                std::vector<Vertex> vertices;

                const auto& position = gltfPrimitive.attributes.find("POSITION");

                vertices.resize(model.accessors[position->second].count);

                for (const auto&[name, index] : gltfPrimitive.attributes)
                {
                    const auto& accessor = model.accessors[index];
                    const auto& view = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[view.buffer];

                    const auto stride = accessor.ByteStride(view) / sizeof(float);
                    const auto data = reinterpret_cast<const float*>(buffer.data.data() + accessor.byteOffset + view.byteOffset);
                    const auto count = static_cast<uint32_t>(accessor.count);

                    for (size_t j = 0; j < count; j++)
                    {
                        auto& [position, normal, uv] = vertices[j];

                        if (name == "POSITION")
                        {
                            position = glm::make_vec3(&data[j * stride]);
                        }
                        else if (name == "NORMAL")
                        {
                            normal = glm::make_vec3(&data[j * stride]);
                        }
                        else if (name == "TEXCOORD_0")
                        {
                            uv = glm::make_vec2(&data[j * stride]);
                        }
                    }
                }

                primitive->SetVertices(std::move(vertices));

                auto indexType = VK_INDEX_TYPE_MAX_ENUM;

                if (gltfPrimitive.indices >= 0)
                {
                    std::vector<uint8_t> indices;
                    const auto& accessor = model.accessors[gltfPrimitive.indices];
                    const auto& view = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[view.buffer];

                    const uint8_t* data = buffer.data.data() + accessor.byteOffset + view.byteOffset;

                    const auto size = accessor.ByteStride(view) * accessor.count;

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

                    primitive->SetIndices(std::move(indices), indexType);
                }

                if (materials[gltfPrimitive.material])
                {
                    primitive->SetMaterial(materials[gltfPrimitive.material]);
                }

                mesh->AddPrimitive(std::move(primitive));
            }

            meshes.push_back(mesh);
        }

        return meshes;
    }

    std::vector<Entity::Id> GltfModule::ImportEntities(tinygltf::Model& model, std::vector<std::shared_ptr<Mesh>>& meshes, Scene& scene)
    {
        std::vector<Entity::Id> entities;

        for (auto& gltfNode : model.nodes)
        {
            auto entity = scene.CreateEntity();

            if (gltfNode.mesh >= 0)
            {
                scene.AddComponent<Component::MeshRender>(entity, meshes[gltfNode.mesh]);
            }

            auto& transform = scene.GetComponent<Component::Transform>(entity);

            if (!gltfNode.translation.empty())
            {
                std::ranges::transform(gltfNode.translation, glm::value_ptr(transform.position), [](auto value) { return static_cast<float>(value); });
            }

            if (!gltfNode.rotation.empty())
            {
                std::ranges::transform(gltfNode.rotation, glm::value_ptr(transform.rotation), [](auto value) { return static_cast<float>(value); });
            }

            if (!gltfNode.name.empty())
            {
                scene.GetComponent<Component::Name>(entity).name = gltfNode.name;
            }

            entities.push_back(entity);
        }

        return entities;
    }

    void GltfModule::SetupRelationship(const std::string& name, const tinygltf::Model& gltfModel, Scene& scene, std::vector<Entity::Id>& entities)
    {
        auto root = scene.CreateEntity();
        scene.GetComponent<Component::Name>(root).name = name;

        for (auto& gltfScene : gltfModel.scenes)
        {
            std::queue<std::pair<Entity::Id, int>> traverseNodes;

            for (auto index : gltfScene.nodes)
            {
                traverseNodes.emplace(root, index);
            }

            while (! traverseNodes.empty())
            {
                auto&[first, second] = traverseNodes.front();
                traverseNodes.pop();

                auto& current = entities[second];
                const auto& parent = first;

                scene.SetParent(current, parent);

                for (auto index: gltfModel.nodes[second].children)
                {
                    traverseNodes.emplace(current, index);
                }
            }
        }
    }
};