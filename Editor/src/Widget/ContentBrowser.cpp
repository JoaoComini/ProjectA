#include "ContentBrowser.hpp"

#include "Resource/ResourceManager.hpp"
#include "Resource/Prefab.hpp"
#include "Resource/Importer/TextureImporter.hpp"
#include "Project/Project.hpp"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

ContentBrowser::ContentBrowser(Vulkan::Device& device, Engine::Scene& scene)
	: device(device), scene(scene)
{
	baseDirectory = Engine::Project::GetResourceDirectory();
	resourceTree.SetRoot(baseDirectory);

	currentDirectory = "";

	Engine::TextureImporter importer{ device };

	fileIconTexture = importer.Import("resources/icons/file.png");
	fileIconDescriptor = ImGui_ImplVulkan_AddTexture(fileIconTexture->GetSampler().GetHandle(), fileIconTexture->GetImageView().GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	directoryIconTexture = importer.Import("resources/icons/directory.png");
	directoryIconDescriptor = ImGui_ImplVulkan_AddTexture(directoryIconTexture->GetSampler().GetHandle(), directoryIconTexture->GetImageView().GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	RefreshResourceTree();
}

ContentBrowser::~ContentBrowser()
{
	device.WaitIdle();
}

void ContentBrowser::Draw()
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
					Engine::Scene scene;
					Engine::ResourceManager::Get().CreateResource<Engine::Scene>(currentDirectory / "untitled", scene);

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

	ContentBrowserItemIcon(filename, directoryIconDescriptor);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		currentDirectory /= path;
	}

	ImGui::TextWrapped(filename.c_str());

	ImGui::PopID();
}

bool ContentBrowser::ContentBrowserFile(std::filesystem::path path, ResourceTree::Node* node)
{
	bool deleted = false;

	std::string filename = path.stem().generic_string();

	ImGui::PushID(filename.c_str());

	ContentBrowserItemIcon(filename, fileIconDescriptor);

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

	ImGui::TextWrapped(filename.c_str());

	ImGui::PopID();

	return deleted;
}

void ContentBrowser::ContentBrowserItemIcon(std::string label, VkDescriptorSet iconDescriptor)
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::ImageButton(label.c_str(), iconDescriptor, { itemSize, itemSize });
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
