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
#include "Resource/Factory/PrefabFactory.hpp"

#include "Resource/ResourceManager.hpp"

#include "Project/Project.hpp"

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
        tinygltf::Model model = LoadModel(path);

        auto directory = GetPrefabDirectory(path);

        if (!std::filesystem::create_directory(directory))
        {
            return;
        }
    
        auto textures = ImportTextures(directory, model);

        auto materials = ImportMaterials(directory, model, textures);

        auto meshes = ImportMeshes(directory, model, materials);

        auto nodes =  ImportNodes(model, meshes);
        
        ImportPrefab(directory, model, nodes);
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

            auto path = GetResourcePath(parent, "texture");

            auto id = factory.Create(path, spec);

            ResourceMetadata metadata
            {
                .path = std::filesystem::relative(path, Project::GetResourceDirectory()),
                .type = ResourceType::Texture
            };

            ResourceRegistry::Get().ResourceCreated(id, metadata);

            textures.push_back(id);
        }

        return textures;
    }

    std::vector<ResourceId> GltfImporter::ImportMaterials(std::filesystem::path parent, tinygltf::Model& model, std::vector<ResourceId>& textures)
    {
        std::vector<ResourceId> materials;

        MaterialFactory factory;

        for (size_t i = 0; i < model.materials.size(); i++)
        {
            auto& material = model.materials[i];
            auto& values = material.values;

            ResourceId diffuse{ 0 };

            if (values.find("baseColorTexture") != values.end()) {
                auto index = values["baseColorTexture"].TextureIndex();

                diffuse = textures[index];
            }

            MaterialSpec spec
            {
                .diffuse = diffuse
            };

            auto path = GetResourcePath(parent, "material");

            auto id = factory.Create(path, spec);

            ResourceMetadata metadata
            {
                .path = std::filesystem::relative(path, Project::GetResourceDirectory()),
                .type = ResourceType::Material
            };

            ResourceRegistry::Get().ResourceCreated(id, metadata);

            materials.push_back(id);
        }

        return materials;
    }

    std::vector<ResourceId> GltfImporter::ImportMeshes(std::filesystem::path parent, tinygltf::Model& model, std::vector<ResourceId>& materials)
    {
        std::vector<ResourceId> meshes;

        MeshFactory factory{ device };

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

            auto path = GetResourcePath(parent, mesh.name);

            auto id = factory.Create(path, spec);

            ResourceMetadata metadata
            {
                .path = std::filesystem::relative(path, Project::GetResourceDirectory()),
                .type = ResourceType::Mesh
            };

            ResourceRegistry::Get().ResourceCreated(id, metadata);

            meshes.push_back(id);
        }

        return meshes;
    }

    std::vector<std::unique_ptr<Node>> GltfImporter::ImportNodes(tinygltf::Model& gltfModel, std::vector<ResourceId>& meshes)
    {
        std::vector<std::unique_ptr<Node>> nodes;

        for (auto& gltfNode : gltfModel.nodes)
        {
            auto node = std::make_unique<Node>();

            if (gltfNode.mesh >= 0)
            {
                node->SetMesh(meshes[gltfNode.mesh]);
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

        return nodes;
    }

    void GltfImporter::ImportPrefab(std::filesystem::path parent, tinygltf::Model& gltfModel, std::vector<std::unique_ptr<Node>>& nodes)
    {
        PrefabFactory factory;
            
        tinygltf::Scene scene = gltfModel.scenes[gltfModel.defaultScene];

        auto root = std::make_unique<Node>();
        root->SetName(scene.name);

        auto model = std::make_shared<Prefab>();
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

            parent.AddChild(current);

            for (auto childIndex : gltfModel.nodes[nodeIt.second].children)
            {
                traverseNodes.push(std::make_pair(std::ref(current), childIndex));
            }
        }

        nodes.push_back(std::move(root));

        model->SetNodes(std::move(nodes));

        std::filesystem::path path = GetResourcePath(parent, parent.stem().string());

        auto id = factory.Create(path, *model);

        ResourceMetadata metadata
        {
            .path = std::filesystem::relative(path, Project::GetResourceDirectory()),
            .type = model->GetType()
        };

        ResourceRegistry::Get().ResourceCreated(id, metadata);
    }

    std::filesystem::path GltfImporter::GetResourcePath(std::filesystem::path parent, std::string name)
    {
        auto base = Project::GetResourceDirectory() / parent.stem();

        std::filesystem::path path;
        ResourceId existing;
        auto current = 0;
        do
        {
            path = base / name;

            if (current > 0)
            {
                path += "_" + std::to_string(current);
            }

            path.replace_extension(".pares");
            current += 1;


            auto relative = std::filesystem::relative(path, Project::GetResourceDirectory());
            existing = ResourceRegistry::Get().FindResourceByPath(relative);
        } while (existing);

        return path;
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