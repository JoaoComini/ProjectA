#pragma once

#include <imgui.h>

#include <Scene/Scene.h>

#include "Widget.h"
#include "Controls.h"

class EntityInspector: public Widget
{
public:
	EntityInspector(Engine::Scene& scene);

    void SetEntity(Engine::Entity::Id entity);
    void Draw() override;

private:
	Engine::Scene& scene;
    Engine::Entity::Id entity;

    template<typename T>
    void AddComponentMenuItem(std::string label)
    {
        if (!scene.HasComponent<T>(entity))
        {
            if (ImGui::MenuItem(label.c_str()))
            {
                scene.AddComponent<T>(entity);
                ImGui::CloseCurrentPopup();
            }
        }
    }

	template<typename T>
	void ComponentControlNode(std::string label, T* component)
	{
		ImGui::Separator();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Framed;
		bool open = ImGui::TreeNodeEx(label.c_str(), flags, "%s", label.c_str());

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
			scene.RemoveComponent<T>(entity);
		}
	}
};