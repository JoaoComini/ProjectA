#pragma once

#include <Scene/Scene.hpp>

#include <imgui.h>

#include "Widget.hpp"

class SceneHierarchy: public Widget
{
public:
	SceneHierarchy(Engine::Scene& scene);
	~SceneHierarchy() = default;

	void Draw() override;

	template<typename T>
	void AddChildNode(Engine::Node* parent)
	{
		auto label = T::GetStaticClass();
		if (ImGui::MenuItem(label.data()))
		{
			parent->AddChild(new T());

			ImGui::CloseCurrentPopup();
		}
	}

	void OnSelectNode(std::function<void(Engine::Node*)> onSelectNodeFn);

private:
	void DrawNode(Engine::Node* node);

	Engine::Scene& scene;
	Engine::Node* selectedNode{ nullptr };

	std::function<void(Engine::Node*)> onSelectNodeFn;
};