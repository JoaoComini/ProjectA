#include "SceneHierarchy.h"

#include <Scene/Scene.h>

#include <imgui.h>

void SceneHierarchy::Draw(Engine::Scene& scene)
{
	ImGui::Begin("Scene");

	{
		auto query = scene.Query<Engine::Entity::Id>(Engine::Exclusion<Engine::Component::Hierarchy>{});

		for (const auto entity : query)
		{
			EntityNode(scene, entity);
		}
	}

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

void SceneHierarchy::EntityNode(Engine::Scene& scene, Engine::Entity::Id entity)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	auto children = scene.TryGetComponent<Engine::Component::Children>(entity);

	if (! children)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (selectedEntity == entity)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	const auto& name = scene.GetComponent<Engine::Component::Name>(entity);

	auto id = (void*)(uint64_t)entity;

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

			scene.SetParent(empty, entity);
		}

		if (ImGui::MenuItem("Destroy Entity"))
		{
			scene.DestroyEntity(entity);

			if (onSelectEntityFn != nullptr)
			{
				onSelectEntityFn(Engine::Entity::Null);
			}
		}

		ImGui::EndPopup();
	}

	ImGui::PopID();

	if (! open)
	{
		return;
	}

	if (children)
	{
		auto current = children->first;

		while (scene.Valid(current))
		{
			EntityNode(scene, current);

			current = scene.GetComponent<Engine::Component::Hierarchy>(current).next;
		}
	}

	ImGui::TreePop();
}

void SceneHierarchy::OnSelectEntity(std::function<void(Engine::Entity::Id)> onSelectEntityFn)
{
	this->onSelectEntityFn = onSelectEntityFn;
}