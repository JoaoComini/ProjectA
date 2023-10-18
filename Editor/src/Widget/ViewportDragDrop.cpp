#include "ViewportDragDrop.hpp"

#include "imgui.h"

#include "Resource/ResourceRegistry.hpp"

void ViewportDragDrop::Draw()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("DragAndDrop", nullptr, flags);

	ImGui::Dummy(ImGui::GetContentRegionAvail());
	if (ImGui::BeginDragDropTarget())
	{
		if (const auto payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			Engine::ResourceId id = *static_cast<Engine::ResourceId*>(payload->Data);

			if (auto metadata = Engine::ResourceRegistry::Get().FindMetadataById(id))
			{
				if (onDropResourceFn)
				{
					onDropResourceFn(id, *metadata);
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
