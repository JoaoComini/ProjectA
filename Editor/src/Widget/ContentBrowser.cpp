#include "ContentBrowser.hpp"

#include "Resource/ResourceManager.hpp"
#include "Resource/Model.hpp"
#include "Resource/Importer/TextureImporter.hpp"
#include "Project/Project.hpp"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

ContentBrowser::ContentBrowser(Vulkan::Device& device, Engine::Scene& scene)
	: device(device), scene(scene)
{
	baseDirectory = Engine::Project::GetResourceDirectory();
	currentDirectory = baseDirectory;

	Engine::TextureImporter importer{ device };

	fileIconTexture = importer.Import("resources/icons/file.png");
	fileIconDescriptor = ImGui_ImplVulkan_AddTexture(fileIconTexture->GetSampler().GetHandle(), fileIconTexture->GetImageView().GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	directoryIconTexture = importer.Import("resources/icons/directory.png");
	directoryIconDescriptor = ImGui_ImplVulkan_AddTexture(directoryIconTexture->GetSampler().GetHandle(), directoryIconTexture->GetImageView().GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	resourceTree.SetRoot(".");
}

ContentBrowser::~ContentBrowser()
{
	device.WaitIdle();
}

void AddEntity(Engine::Scene& scene, Engine::Node& node, Engine::Entity* parent)
{
	auto entity = scene.CreateEntity();

	auto& transform = entity.AddComponent<Engine::Component::Transform>();
	transform.position = node.GetTransform().position;
	transform.rotation = node.GetTransform().rotation;
	transform.scale = node.GetTransform().scale;

	if (node.GetMesh())
	{
		entity.AddComponent<Engine::Component::MeshRender>().mesh = node.GetMesh();
	}

	entity.GetComponent<Engine::Component::Name>().name = node.GetName();

	if (parent)
	{
		entity.SetParent(*parent);
	}

	for (auto& child : node.GetChildren())
	{
		AddEntity(scene, *child, &entity);
	}
}

void ContentBrowser::Draw()
{
	RefreshResourceTree();

	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowSizeConstraints({ (itemSize + style.ColumnsMinSpacing) * 2.5f, 0 }, viewport->Size);

	if (!ImGui::Begin("Content Browser"))
	{
		ImGui::End();
		return;
	}

	ContentBrowserBackButton();

	ContentBrowserTable();

	ImGui::End();
}

void ContentBrowser::ContentBrowserBackButton()
{
	bool disabled = currentDirectory == baseDirectory;

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
	if (ImGui::BeginTable("ContentTable", GetContentTableColumns(), ImGuiTableFlags_SizingFixedFit))
	{
		ImGui::TableSetupColumn("ContentTableColumn", ImGuiTableColumnFlags_WidthFixed);

		auto currentNode = resourceTree.Search(std::filesystem::relative(currentDirectory, baseDirectory));

		for (auto& [path, node] : currentNode->children)
		{
			ImGui::TableNextColumn();

			bool isDirectory = std::filesystem::is_directory(baseDirectory / path);

			if (isDirectory)
			{
				ContentBrowserDirectory(path);
			}
			else
			{
				ContentBrowserFile(path, node);
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
		currentDirectory /= path.filename();
	}

	ImGui::TextWrapped(filename.c_str());

	ImGui::PopID();
}

void ContentBrowser::ContentBrowserFile(std::filesystem::path path, ResourceTree::Node* node)
{
	std::string filename = path.filename().generic_string();

	ImGui::PushID(filename.c_str());

	ContentBrowserItemIcon(filename, fileIconDescriptor);

	if (ImGui::BeginPopupContextItem())
	{
		if (node->metadata.type == Engine::ResourceType::Model && ImGui::MenuItem("Add to Scene"))
		{
			auto model = Engine::ResourceManager::GetInstance()->LoadResource<Engine::Model>(node->id);

			for (auto child : model->GetRoot().GetChildren())
			{
				AddEntity(scene, *child, nullptr);
			}
		}

		ImGui::EndPopup();
	}

	ImGui::TextWrapped(filename.c_str());

	ImGui::PopID();
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
	auto& registry = Engine::ResourceManager::GetInstance()->GetResourceRegistry();

	for (auto& [id, metadata] : registry)
	{
		resourceTree.Refresh(metadata.path, id, metadata);
	}
}