#pragma once

#include "SceneImporter.h"
#include "Scene/SceneGraph.h"

namespace tinygltf
{
	class Model;
}

namespace Engine
{
	class GltfModule final : public SceneImporterModule
	{
	public:
		std::unique_ptr<SceneResource> Import(const std::filesystem::path& source) override;
		[[nodiscard]] std::vector<std::string> GetImportExtensions() const override;
	};
};