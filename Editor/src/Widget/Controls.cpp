#include "Controls.hpp"

#include <imgui.h>
#include <glm/glm.hpp>

#include <Scene/Node/TransformNode.hpp>
#include <Scene/Node/LightNode.hpp>

#include <Rendering/Camera.hpp>

#include <Resource/ResourceManager.hpp>

namespace Controls
{
	void Vec3(const std::string& label, glm::vec3& values, float resetValue)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0, 0 });
		if (ImGui::BeginTable(label.c_str(), 4))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

			ImGui::TableNextColumn();

			ResetButton("X", &values.x, ButtonCollors{
				.normal = ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f },
				.hovered = ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f },
				.active = ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f }
			}, resetValue);

			ImGui::SameLine();

			Float("##X", &values.x);

			ImGui::TableNextColumn();

			ResetButton("Y", &values.y, ButtonCollors{
				.normal = ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f },
				.hovered = ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f },
				.active = ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f }
			}, resetValue);

			ImGui::SameLine();

			Float("##Y", &values.y);

			ImGui::TableNextColumn();

			ResetButton("Z", &values.z, ButtonCollors{
				.normal = ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f },
				.hovered = ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f },
				.active = ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f }
			}, resetValue);

			ImGui::SameLine();

			Float("##Z", &values.z);

			ImGui::TableNextColumn();

			ImGui::Text(label.c_str());

			ImGui::PopStyleVar();
			ImGui::EndTable();
		}
		ImGui::PopStyleVar();
	}

	void ResetButton(const std::string& label, float* value, ButtonCollors colors, float resetValue)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, colors.active);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors.hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors.normal);
		if (ImGui::Button(label.c_str()))
		{
			*value = resetValue;
		}
		ImGui::PopStyleColor(3);
	}

	bool Float(const std::string& label, float* value, float min, float max)
	{
		return ImGui::DragFloat(label.c_str(), value, 0.1f, min, max, "%.2f");
	}

	template<>
	void Node(Engine::TransformNode* node)
	{
		auto position = node->GetPosition();
		auto rotation = glm::degrees(node->GetRotation());
		auto scale = node->GetScale();

		Controls::Vec3("Position", position);
		Controls::Vec3("Rotation", rotation);
		Controls::Vec3("Scale", scale, 1.f);

		node->SetPosition(position);
		node->SetRotation(glm::radians(rotation));
		node->SetScale(scale);
	}

	template<>
	void Node(Engine::DirectionalLightNode* node)
	{
		float intensity = node->GetIntensity();
		glm::vec3 color = node->GetColor();

		ImGui::ColorEdit4("Color", &color.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
		ImGui::SliderFloat("Intensity", &intensity, 0.1f, 5.f);

		node->SetIntensity(intensity);
		node->SetColor(color);
	}

	template<>
	void Node(Engine::PointLightNode* node)
	{
		float range = node->GetRange();
		glm::vec3 color = node->GetColor();

		ImGui::ColorEdit4("Color", &color.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
		ImGui::SliderFloat("Range", &range, 0.1f, 1000.f);

		node->SetRange(range);
		node->SetColor(color);
	}

	//template<>
	//void Component(Engine::Component::Camera* component)
	//{
	//	auto camera = dynamic_cast<Engine::PerspectiveCamera *>(component->camera);

	//	float fov = glm::degrees(camera->GetFov());
	//	float near = camera->GetNear();
	//	float far = camera->GetFar();

	//	if (Controls::Float("FOV", &fov))
	//	{
	//		camera->SetFov(glm::radians(fov));
	//	}

	//	if (Controls::Float("Near", &near, 0.1f))
	//	{
	//		camera->SetNear(near);
	//	}

	//	if (Controls::Float("Far", &far, 0.1f))
	//	{
	//		camera->SetFar(far);
	//	}
	//}
};