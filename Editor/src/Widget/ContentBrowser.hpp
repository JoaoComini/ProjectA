#pragma once

#include "Widget.hpp"

#include "Vulkan/Device.hpp"

#include "Rendering/Texture.hpp"

#include "Scene/Scene.hpp"

#include <filesystem>

class ResourceTree
{
public:
	struct Node
	{
		Node(std::filesystem::path path, Engine::ResourceId id = { 0 }, Engine::ResourceMetadata metadata = {}) 
			: path(path), id(id), metadata(metadata) {}

		std::filesystem::path path;
		Engine::ResourceId id;
		Engine::ResourceMetadata metadata;

		Node* parent = nullptr;
		std::map<std::filesystem::path, Node*> children;
	};

	void SetRoot(std::filesystem::path path)
	{
		if (root == nullptr)
		{
			root = new Node(path);
		}
	}

	Node* Search(std::filesystem::path path)
	{
		Node* node = root;

		for (const auto& p : path)
		{
			if (node->path == path)
			{
				return node;
			}

			if (node->children.find(p) != node->children.end())
			{
				node = node->children[p];
			}
			else
			{
				return nullptr;
			}
		}

		return node;
	}
	
	void Refresh(std::filesystem::path path, Engine::ResourceId id, Engine::ResourceMetadata metadata)
	{
		auto currentNode = root;

		for (const auto& p : path)
		{
			auto it = currentNode->children.find(p.generic_string());
			if (it != currentNode->children.end())
			{
				currentNode = it->second;
				continue;
			}

			auto node = new Node(p, id, metadata);

			AddNode(currentNode, node);

			currentNode = node;
		}
	}

private:

	void AddNode(Node* parent, Node* node)
	{
		node->parent = parent;
		parent->children[node->path] = node;
	}

	Node* root = nullptr;
};

class ContentBrowser : public Widget
{
public:
	ContentBrowser(Vulkan::Device& device, Engine::Scene& scene);
	~ContentBrowser();

	virtual void Draw() override;
private:
	void ContentBrowserBackButton();

	void ContentBrowserTable();
	void ContentBrowserDirectory(std::filesystem::path path);
	void ContentBrowserFile(std::filesystem::path path, ResourceTree::Node* node);
	void ContentBrowserItemIcon(std::string label, VkDescriptorSet iconDescriptor);

	int GetContentTableColumns();

	void RefreshResourceTree();

	std::filesystem::path currentDirectory;
	std::filesystem::path baseDirectory;

	std::shared_ptr<Engine::Texture> fileIconTexture;
	VkDescriptorSet fileIconDescriptor;

	std::shared_ptr<Engine::Texture> directoryIconTexture;
	VkDescriptorSet directoryIconDescriptor;

	ResourceTree resourceTree;
	
	const Vulkan::Device& device;
	Engine::Scene& scene;

	float itemSize = 96;
};

