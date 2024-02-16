#include "SceneHierarchy.hpp"

#include <Scene/Node/TransformNode.hpp>
#include <Scene/Node/MeshNode.hpp>
#include <Scene/Node/SkyLightNode.hpp>
#include <Scene/Node/LightNode.hpp>

#include <imgui.h>

SceneHierarchy::SceneHierarchy(Engine::Scene& scene)
    : scene(scene) {}

void SceneHierarchy::Draw()
{
	ImGui::Begin("Scene");

	if (ImGui::Button("Add Child"))
	{
		ImGui::OpenPopup("##AddChild");
	}

	if (ImGui::BeginPopup("##AddChild"))
	{
		AddChildNode<Engine::Node>(scene.GetRoot());
		AddChildNode<Engine::TransformNode>(scene.GetRoot());
		AddChildNode<Engine::MeshNode>(scene.GetRoot());
		AddChildNode<Engine::SkyLightNode>(scene.GetRoot());
		AddChildNode<Engine::DirectionalLightNode>(scene.GetRoot());
		AddChildNode<Engine::PointLightNode>(scene.GetRoot());
		ImGui::EndPopup();
	}

	for (auto child: scene.GetRoot()->GetChildren())
	{
		DrawNode(child);
	}

	ImGui::End();
}

void SceneHierarchy::DrawNode(Engine::Node* node)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	auto children = node->GetChildren();

	if (children.size() == 0)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (selectedNode == node)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	auto name = node->GetName();

	auto id = (void*)(uint64_t)node;

	bool open = ImGui::TreeNodeEx(id, flags, name.data());

	if (ImGui::IsItemClicked())
	{
		selectedNode = node;

		if (onSelectNodeFn != nullptr)
		{
			onSelectNodeFn(selectedNode);
		}
	}

	ImGui::PushID(id);
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::BeginMenu("Add Child"))
		{
			AddChildNode<Engine::Node>(node);
			AddChildNode<Engine::TransformNode>(node);
			AddChildNode<Engine::MeshNode>(node);
			AddChildNode<Engine::SkyLightNode>(node);
			AddChildNode<Engine::DirectionalLightNode>(node);
			AddChildNode<Engine::PointLightNode>(node);
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Remove Node"))
		{
			if (auto parent = node->GetParent())
			{
				parent->RemoveChild(node);
			}

			if (onSelectNodeFn != nullptr)
			{
				onSelectNodeFn(nullptr);
			}
		}

		ImGui::EndPopup();
	}
	ImGui::PopID();

	if (open)
	{
		for (auto child : children)
		{
			DrawNode(child);
		}

		ImGui::TreePop();
	}
}

void SceneHierarchy::OnSelectNode(std::function<void(Engine::Node*)> onSelectNodeFn)
{
	this->onSelectNodeFn = onSelectNodeFn;
}
