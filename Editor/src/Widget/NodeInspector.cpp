#include "NodeInspector.hpp"

#include <Scene/Node/TransformNode.hpp>
#include <Scene/Node/LightNode.hpp>

#include <misc/cpp/imgui_stdlib.h>

#include "Controls.hpp"

void NodeInspector::SetNode(Engine::Node* node)
{
	this->node = node;
}

void NodeInspector::Draw()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSizeConstraints({ 350, 0 }, viewport->Size);

	ImGui::Begin("Inspector");

	if (!node)
	{
		ImGui::End();
		return;
	}

	auto name = node->GetName();

	if (ImGui::InputText("##Name", &name))
	{
		node->SetName(name);
	}

	if (auto transform = dynamic_cast<Engine::TransformNode*>(node))
	{
		Controls::Node<Engine::TransformNode>(transform);
	}

	if (auto light = dynamic_cast<Engine::DirectionalLightNode*>(node))
	{
		Controls::Node<Engine::DirectionalLightNode>(light);
	}
	else if (auto light = dynamic_cast<Engine::PointLightNode*>(node))
	{
		Controls::Node<Engine::PointLightNode>(light);
	}

	//auto camera = entity.TryGetComponent<Engine::Component::Camera>();

	//if (camera)
	//{
	//	ComponentControlNode<Engine::Component::Camera>("Camera", camera);
	//}

	ImGui::End();
}
