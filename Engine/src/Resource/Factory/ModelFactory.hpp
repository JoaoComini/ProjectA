#pragma once

#include "Vulkan/Device.hpp"

#include "Resource/Model.hpp"

#include <filesystem>

namespace Engine
{
	class ModelFactory
	{
	public:
		ResourceId Create(std::filesystem::path destination, Model& model);

		void WriteNode(std::ofstream& file, Node& node);

		std::shared_ptr<Model> Load(std::filesystem::path source);

		void ReadNodes(std::ifstream& file, Node* parent, std::vector<std::unique_ptr<Node>>& nodes);
	};
};