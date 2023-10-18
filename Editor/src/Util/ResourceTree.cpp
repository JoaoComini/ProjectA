#include "ResourceTree.hpp"

#include <filesystem>


void ResourceTree::SetRoot(std::filesystem::path path)
{
	if (root == nullptr)
	{
		root = new Node(path, true);
	}
}

ResourceTree::Node* ResourceTree::Search(std::filesystem::path path)
{
	if (path.empty())
	{
		return root;
	}

	ResourceTree::Node* node = root;

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

void ResourceTree::Refresh(std::filesystem::path path, Engine::ResourceId id, Engine::ResourceMetadata metadata)
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

		auto directory = std::filesystem::is_directory(root->path / p);

		auto node = new Node(p, directory, id, metadata);

		AddNode(currentNode, node);

		currentNode = node;
	}
}

void ResourceTree::AddNode(Node* parent, Node* node)
{
	node->parent = parent;
	parent->children[node->path] = node;
}

void ResourceTree::DeleteNode(Node* node)
{
	node->parent->children.erase(node->path);

	for (auto& [_, child] : node->children)
	{
		DeleteNode(child);
	}
}