#pragma once

#include <imgui.h>

#include <Scene/Entity.hpp>
#include <Scene/Components.hpp>

#include "Widget.hpp"
#include "Controls.hpp"

class EntityInspector: public Widget
{
public:

    void SetEntity(Engine::Entity entity);
    void Update() override;

private:
    Engine::Entity entity;

    template<typename T>
    void AddComponentMenuItem(std::string label)
    {
        if (!entity.HasComponent<T>())
        {
            if (ImGui::MenuItem(label.c_str()))
            {
                entity.AddComponent<T>();
                ImGui::CloseCurrentPopup();
            }
        }
    }

	template<typename T>
	void ComponentControlNode(std::string label, T* component)
	{
		ImGui::Separator();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Framed;
		bool open = ImGui::TreeNodeEx(label.c_str(), flags, label.c_str());

		bool deleted = false;
		if (typeid(T) != typeid(Engine::Component::Transform))
		{
			auto frameHeight = ImGui::GetFrameHeight();
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - (frameHeight + 8) * 0.5f);

			if (ImGui::Button("...", { frameHeight + 8, frameHeight }))
			{
				ImGui::OpenPopup("ComponentMenu");
			}


			if (ImGui::BeginPopup("ComponentMenu"))
			{
				if (ImGui::MenuItem("Delete Component"))
				{
					deleted = true;
				}

				ImGui::EndPopup();
			}
		}

		if (open)
		{
			Controls::Component(component);
			ImGui::TreePop();
		}

		if (deleted)
		{
			entity.RemoveComponent<T>();
		}
	}
};