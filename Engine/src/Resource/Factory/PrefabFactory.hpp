#pragma once

#include "Vulkan/Device.hpp"

#include "Resource/Prefab.hpp"

#include "Factory.hpp"

#include "../Flatbuffers/Prefab_generated.h"


namespace Engine
{
	class PrefabFactory : Factory<Prefab, Prefab>
	{
	public:
		void Create(std::filesystem::path destination, Prefab& model) override;

		std::shared_ptr<Prefab> Load(std::filesystem::path source) override;

	private:
		flatbuffers::Offset<flatbuffers::Node> WriteNode(flatbuffers::FlatBufferBuilder& builder, Node& node);
		void ReadNodes(flatbuffers::NodeT& buffer, Node* parent, std::vector<std::unique_ptr<Node>>& nodes);

	};
};