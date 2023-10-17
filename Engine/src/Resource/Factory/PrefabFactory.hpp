#pragma once

#include "Vulkan/Device.hpp"

#include "Resource/Prefab.hpp"

#include "../Flatbuffers/Prefab_generated.h"

#include <filesystem>

namespace Engine
{
	class PrefabFactory
	{
	public:
		ResourceId Create(std::filesystem::path destination, Prefab& model);

		std::shared_ptr<Prefab> Load(std::filesystem::path source);

	private:
		flatbuffers::Offset<flatbuffers::Node> WriteNode(flatbuffers::FlatBufferBuilder& builder, Node& node);
		void ReadNodes(flatbuffers::NodeT& buffer, Node* parent, std::vector<std::unique_ptr<Node>>& nodes);

	};
};