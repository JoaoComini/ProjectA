#pragma once

#include "Widget.hpp"

#include "Util/ResourceTree.hpp"

#include "Vulkan/Device.hpp"

#include "Rendering/Texture.hpp"

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
	void ContentBrowserItemIcon(std::string label, VkDescriptorSet iconDescriptor);

	int GetContentTableColumns();

	std::filesystem::path currentDirectory;
	std::filesystem::path baseDirectory;
	ResourceTree resourceTree;

	std::shared_ptr<Engine::Texture> fileIconTexture;
	VkDescriptorSet fileIconDescriptor;

	std::shared_ptr<Engine::Texture> directoryIconTexture;
	VkDescriptorSet directoryIconDescriptor;

	float itemSize = 96;

	std::function<void(Engine::ResourceId, Engine::ResourceMetadata)> onResourceDoubleClick;
		
	const Vulkan::Device& device;
	Engine::Scene& scene;
};

