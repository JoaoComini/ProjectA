#include "ContentBrowser.h"

#include "Resource/ResourceManager.h"
#include "Resource/Importer/TextureImporter.h"
#include "Project/Project.h"
#include "Scene/Scene.h"

#include "Scripting/Script.h"

#include <Icons/embed.gen.hpp>

#include <imgui.h>

ContentBrowser::ContentBrowser(Vulkan::Device& device)
	: device(device)
{
	baseDirectory = Engine::Project::GetResourceDirectory();
	resourceTree.SetRoot(baseDirectory);

	currentDirectory = "";

	Engine::TextureImporter importer{};

	auto fileIcon = embed::Icons::get("file.png");

	fileIconTexture = importer.LoadDefault(std::vector<uint8_t>{ fileIcon.begin(), fileIcon.end() });
	fileIconTexture->UploadToGpu(device);

	auto directoryIcon = embed::Icons::get("directory.png");

	directoryIconTexture = importer.LoadDefault(std::vector<uint8_t>{ directoryIcon.begin(), directoryIcon.end() });
	directoryIconTexture->UploadToGpu(device);

	RefreshResourceTree();
}

ContentBrowser::~ContentBrowser()
{
	device.WaitIdle();
}

void ContentBrowser::OnResourceDoubleClick(std::function<void(Engine::ResourceId, Engine::ResourceMapping)> onResourceDoubleClick)
{
	this->onResourceDoubleClick = onResourceDoubleClick;
}

void ContentBrowser::Draw(Engine::Scene& scene)
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowSizeConstraints({ (itemSize + style.ColumnsMinSpacing) * 2.5f, 0 }, viewport->Size);

	if (ImGui::Begin("Content Browser"))
	{
		if (ImGui::BeginPopupContextWindow("ContentBrowserMenu", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Scene"))
				{
					Engine::Scene created;
					Engine::ResourceManager::Get().CreateResource<Engine::Scene>(currentDirectory / "untitled.scene", created);

					RefreshResourceTree();
				}

				if (ImGui::MenuItem("Script"))
				{
					Engine::Script script;
					Engine::ResourceManager::Get().CreateResource<Engine::Script>(currentDirectory / "script.lua", script);

					RefreshResourceTree();
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}

		ContentBrowserBackButton();

		ContentBrowserTable();

		ImGui::End();
	}
}

void ContentBrowser::ContentBrowserBackButton()
{
	bool disabled = currentDirectory.empty();

	if (disabled)
	{
		ImGui::BeginDisabled();
	}

	if (ImGui::Button("<-"))
	{
		currentDirectory = currentDirectory.parent_path();
	}

	if (disabled)
	{
		ImGui::EndDisabled();
	}
}

void ContentBrowser::ContentBrowserTable()
{
	auto columns = GetContentTableColumns();

	if (columns <= 0)
	{
		return;
	}

	if (ImGui::BeginTable("ContentTable", columns, ImGuiTableFlags_SizingFixedFit))
	{
		ImGui::TableSetupColumn("ContentTableColumn", ImGuiTableColumnFlags_WidthFixed);

		auto currentNode = resourceTree.Search(currentDirectory);

		auto& children = currentNode->children;

		for (auto it = children.cbegin(), next = it; it != children.cend(); it = next)
		{
			next++;

			auto& [path, node] = *it;
			bool deleted = false;

			ImGui::TableNextColumn();

			if (node->directory)
			{
				ContentBrowserDirectory(path);
			}
			else
			{
				deleted = ContentBrowserFile(path, node);
			}

			if (deleted)
			{
				resourceTree.DeleteNode(node);
			}
		}

		ImGui::EndTable();
	}
}

void ContentBrowser::ContentBrowserDirectory(std::filesystem::path path)
{
	std::string filename = path.filename().generic_string();

	ImGui::PushID(filename.c_str());

	ContentBrowserItemIcon(filename, *directoryIconTexture);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		currentDirectory /= path;
	}

	ImGui::TextWrapped("%s", filename.c_str());

	ImGui::PopID();
}

bool ContentBrowser::ContentBrowserFile(std::filesystem::path path, ResourceTree::Node* node)
{
	bool deleted = false;

	std::string filename = path.stem().generic_string();

	ImGui::PushID(filename.c_str());

	ContentBrowserItemIcon(filename, *fileIconTexture);

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &node->id, sizeof(node->id));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete"))
		{
			Engine::ResourceManager::Get().DeleteResource(node->id);
			
			deleted = true;
		}

		ImGui::EndPopup();
	}

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && onResourceDoubleClick)
	{
		onResourceDoubleClick(node->id, node->mapping);
	}

	ImGui::TextWrapped("%s", filename.c_str());

	ImGui::PopID();

	return deleted;
}

void ContentBrowser::ContentBrowserItemIcon(std::string label, Engine::Texture& texture)
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::ImageButton(label.c_str(), &texture, { itemSize, itemSize });
	ImGui::PopStyleColor();
}

int ContentBrowser::GetContentTableColumns()
{
	ImGuiStyle& style = ImGui::GetStyle();

	float sizeX = ImGui::GetContentRegionAvail().x;
	return sizeX / (itemSize + style.ColumnsMinSpacing + style.CellPadding.x);
}

void ContentBrowser::RefreshResourceTree()
{
	auto& registry = Engine::ResourceRegistry::Get().GetResources();

	for (auto& [id, metadata] : registry)
	{
		resourceTree.Refresh(metadata.path, id, metadata);
	}
}
