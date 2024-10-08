#include "SceneHierarchy.hpp"

#include <imgui.h>

SceneHierarchy::SceneHierarchy(Engine::Scene& scene)
    : scene(scene) {}

void SceneHierarchy::Draw()
{
	ImGui::Begin("Scene");

	scene.ForEachEntity([&](Engine::Entity entity) {
		EntityNode(entity);
	}, Engine::Exclusion<Engine::Component::Hierarchy>{});

	if (ImGui::BeginPopupContextWindow("EntityMenu", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
	{
		if (ImGui::MenuItem("Create Empty Entity"))
		{
			scene.CreateEntity();
		}

		ImGui::EndPopup();
	}

	ImGui::End();
}

void SceneHierarchy::EntityNode(Engine::Entity entity)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	std::vector<Engine::Entity> children = entity.GetChildren();

	if (children.size() == 0)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (selectedEntity == entity)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	auto name = entity.GetComponent<Engine::Component::Name>();

	auto id = (void*)(uint64_t)entity.GetHandle();

	bool open = ImGui::TreeNodeEx(id, flags, "%s", name.name.c_str());

	if (ImGui::IsItemClicked())
	{
		selectedEntity = entity;

		if (onSelectEntityFn != nullptr)
		{
			onSelectEntityFn(selectedEntity);
		}
	}

	ImGui::PushID(id);

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Create Empty Entity"))
		{
			auto empty = scene.CreateEntity();

			empty.SetParent(entity);
		}

		if (ImGui::MenuItem("Destroy Entity"))
		{
			scene.DestroyEntity(entity);

			if (onSelectEntityFn != nullptr)
			{
				onSelectEntityFn(Engine::Entity{});
			}
		}

		ImGui::EndPopup();
	}

	ImGui::PopID();

	if (open)
	{
		for (auto& child : children)
		{
			EntityNode(child);
		}

		ImGui::TreePop();
	}
}

void SceneHierarchy::OnSelectEntity(std::function<void(Engine::Entity)> onSelectEntityFn)
{
	this->onSelectEntityFn = onSelectEntityFn;
}