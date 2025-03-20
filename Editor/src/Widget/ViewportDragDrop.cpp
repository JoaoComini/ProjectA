#include "ViewportDragDrop.h"

#include "Resource/ResourceRegistry.h"

#include <imgui.h>
#include <imgui_internal.h>

void ViewportDragDrop::Draw(Engine::Scene& scene)
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	auto id = ImGui::DockSpaceOverViewport(viewport, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode);

	if (ImGui::GetDragDropPayload() == nullptr)
	{
		return;
	}

	auto node = ImGui::DockBuilderGetCentralNode(id);
	ImGui::SetNextWindowSize(node->Size);
	ImGui::SetNextWindowPos(node->Pos);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("DragAndDrop", nullptr, flags);

	ImGui::Dummy(ImGui::GetContentRegionAvail());
	if (ImGui::BeginDragDropTarget())
	{
		if (const auto payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			Engine::ResourceId resource = *static_cast<Engine::ResourceId*>(payload->Data);

			if (auto mapping = Engine::ResourceRegistry::Get().FindMappingById(resource))
			{
				if (onDropResourceFn)
				{
					onDropResourceFn(resource, *mapping);
				}
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::End();

}

void ViewportDragDrop::OnDropResource(OnDropResourceFn fn)
{
	onDropResourceFn = fn;
}
