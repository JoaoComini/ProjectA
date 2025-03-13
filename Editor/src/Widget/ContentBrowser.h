#pragma once

#include "Widget.h"

#include "Util/ResourceTree.h"

#include "Vulkan/Device.h"

#include "Rendering/Texture.h"

#include <filesystem>

namespace Engine
{
	class Scene;
};

class ContentBrowser : public Widget
{
public:
	ContentBrowser(Vulkan::Device& device, Engine::Scene& scene);
	~ContentBrowser();

	virtual void Draw() override;
	void RefreshResourceTree();

	void OnResourceDoubleClick(std::function<void(Engine::ResourceId, Engine::ResourceMetadata)> onResourceDoubleClick);
private:
	void ContentBrowserBackButton();

	void ContentBrowserTable();
	void ContentBrowserDirectory(std::filesystem::path path);
	bool ContentBrowserFile(std::filesystem::path path, ResourceTree::Node* node);
	void ContentBrowserItemIcon(std::string label, Engine::Texture& texture);

	int GetContentTableColumns();

	std::filesystem::path currentDirectory;
	std::filesystem::path baseDirectory;
	ResourceTree resourceTree;

	std::shared_ptr<Engine::Texture> fileIconTexture;
	std::shared_ptr<Engine::Texture> directoryIconTexture;

	float itemSize = 96;

	std::function<void(Engine::ResourceId, Engine::ResourceMetadata)> onResourceDoubleClick;
		
	const Vulkan::Device& device;
	Engine::Scene& scene;
};

