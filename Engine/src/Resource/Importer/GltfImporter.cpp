#include "GltfImporter.hpp"

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Resource/Factory/TextureFactory.hpp"
#include "Resource/Factory/MaterialFactory.hpp"
#include "Resource/Factory/MeshFactory.hpp"
#include "Resource/Factory/ModelFactory.hpp"

#include "Resource/ResourceManager.hpp"

#include <iostream>
#include <queue>

namespace Engine
{
    GltfImporter::GltfImporter(const Vulkan::Device& device)
        : device(device) 
    {
    }

    void GltfImporter::Import(std::filesystem::path path)
    {
        tinygltf::Model model;
        LoadModel(path, model);


        std::vector<std::shared_ptr<Texture>> textures;
        ImportTextures(path, model, textures);

        std::vector<std::shared_ptr<Material>> materials;
        ImportMaterials(path, model, textures, materials);

        std::vector<std::shared_ptr<Mesh>> meshes;
        ImportMeshes(path, model, materials, meshes);

        std::vector<std::unique_ptr<Node>> nodes;
        ImportNodes(model, meshes, nodes);
        
        ImportModel(path, model, nodes);
    }

    void GltfImporter::LoadModel(std::filesystem::path path, tinygltf::Model& model)
    {
        tinygltf::TinyGLTF loader;

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
    }

    void GltfImporter::ImportTextures(std::filesystem::path parent, tinygltf::Model& model, std::vector<std::shared_ptr<Texture>>& textures)
    {
        TextureFactory factory{ device };

        for (size_t i = 0; i < model.textures.size(); i++)
        {
            auto& gltfTexture = model.textures[i];
            auto& image = model.images[gltfTexture.source];

            TextureSpec spec
            {
                .width = image.width,
                .height = image.height,
                .component = image.component,
                .image = image.image
            };

            auto fileName = gltfTexture.name;

            if (fileName.empty())
            {
                fileName = "texture" + std::to_string(i);
            }

            auto path = GetFilePath(parent, fileName, ".texture");

            auto texture = factory.Create(path, spec);

            ResourceMetadata metadata
            {
                .path = path,
                .type = texture->GetType()
            };

            ResourceManager::GetInstance()->OnResourceImport(texture, metadata);

            textures.push_back(texture);
        }
    }

    void GltfImporter::ImportMaterials(std::filesystem::path parent, tinygltf::Model& model, std::vector<std::shared_ptr<Texture>>& textures, std::vector<std::shared_ptr<Material>>& materials)
    {
        MaterialFactory factory;

        for (auto& material : model.materials)
        {
            auto& values = material.values;

            std::shared_ptr<Engine::Texture> diffuse = nullptr;

            if (values.find("baseColorTexture") != values.end()) {
                auto index = values["baseColorTexture"].TextureIndex();

                diffuse = textures[index];
            }

            MaterialSpec spec
            {
                .diffuse = diffuse->GetId()
            };

            std::filesystem::path path{ material.name + ".material" };

            auto created = factory.Create(path, spec);

            ResourceMetadata metadata
            {
                .path = path,
                .type = created->GetType()
            };

            ResourceManager::GetInstance()->OnResourceImport(created, metadata);

            materials.push_back(created);
        }
    }

    void GltfImporter::ImportMeshes(std::filesystem::path parent, tinygltf::Model& model, std::vector<std::shared_ptr<Material>>& materials, std::vector<std::shared_ptr<Mesh>>& meshes)
    {
        MeshFactory factory{ device };

        for (auto& node : model.nodes)
        {
            if (node.mesh < 0)
            {
                continue;
            }

            auto& mesh = model.meshes[node.mesh];

            for (size_t i = 0; i < mesh.primitives.size(); i++)
            {
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
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        indexType = VK_INDEX_TYPE_UINT16;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        indexType = VK_INDEX_TYPE_UINT32;
                        break;
                    }

                    indices.insert(indices.end(), static_cast<uint8_t*>(data), static_cast<uint8_t*>(data) + size);
                }

                MeshSpec spec
                {
                    .material = materials[primitive.material]->GetId(),
                    .vertices = vertices,
                    .indices = indices,
                    .indexType = indexType
                };

                std::filesystem::path path{ node.name + ".mesh"};

                auto created = factory.Create(path, spec);

                ResourceMetadata metadata
                {
                    .path = path,
                    .type = created->GetType()
                };

                ResourceManager::GetInstance()->OnResourceImport(created, metadata);

                meshes.push_back(created);
            }
        }
    }

    void GltfImporter::ImportNodes(tinygltf::Model& gltfModel, std::vector<std::shared_ptr<Mesh>>& meshes, std::vector<std::unique_ptr<Node>>& nodes)
    {
        for (auto& gltfNode : gltfModel.nodes)
        {
            auto node = std::make_unique<Node>();

            if (gltfNode.mesh >= 0)
            {
                node->SetMesh(meshes[gltfNode.mesh]->GetId());
            }

            auto& transform = node->GetTransform();

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
                node->SetName(gltfNode.name);
            }

            nodes.push_back(std::move(node));
        }
    }

    void GltfImporter::ImportModel(std::filesystem::path parent, tinygltf::Model& gltfModel, std::vector<std::unique_ptr<Node>>& nodes)
    {
        ModelFactory factory;
            
        tinygltf::Scene scene = gltfModel.scenes[gltfModel.defaultScene];

        auto root = std::make_unique<Node>();
        root->SetName(scene.name);

        auto model = std::make_shared<Model>();
        model->SetRoot(*root);

        std::queue<std::pair<Node&, int>> traverseNodes;

        for (auto nodeIndex : scene.nodes)
        {
            traverseNodes.push(std::make_pair(std::ref(*root), nodeIndex));
        }

        while (!traverseNodes.empty())
        {
            auto nodeIt = traverseNodes.front();
            traverseNodes.pop();

            auto& current = *nodes[nodeIt.second];
            auto& parent = nodeIt.first;

            current.SetParent(parent);
            parent.AddChild(current);

            for (auto childIndex : gltfModel.nodes[nodeIt.second].children)
            {
                traverseNodes.push(std::make_pair(std::ref(current), childIndex));
            }
        }

        nodes.push_back(std::move(root));

        model->SetNodes(std::move(nodes));

        std::filesystem::path path{ parent.filename() };
        path.append(".model");

        factory.Create(path, *model);

        ResourceMetadata metadata
        {
            .path = path,
            .type = model->GetType()
        };

        ResourceManager::GetInstance()->OnResourceImport(model, metadata);
    }


    std::filesystem::path GltfImporter::GetFilePath(std::filesystem::path parent, std::filesystem::path path, std::string_view suffix)
    {
        auto filePath = parent.filename().concat("_");

        filePath += path;

        filePath.concat(suffix);

        return filePath;
    }
};