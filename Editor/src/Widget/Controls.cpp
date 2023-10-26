#include "Controls.hpp"

#include <imgui.h>
#include <glm/glm.hpp>

#include <Scene/Components.hpp>

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
	void Component(Engine::Component::Transform* transform)
	{
		glm::vec3 rotation = glm::degrees(glm::eulerAngles(transform->rotation));

		Controls::Vec3("Position", transform->position);
		Controls::Vec3("Rotation", rotation);
		Controls::Vec3("Scale", transform->scale, 1.f);

		transform->rotation = glm::quat(glm::radians(rotation));
	}

	template<>
	void Component(Engine::Component::Camera* component)
	{
		auto camera = dynamic_cast<Engine::PerspectiveCamera *>(component->camera);

		float fov = glm::degrees(camera->GetFov());
		float near = camera->GetNear();
		float far = camera->GetFar();

		if (Controls::Float("FOV", &fov))
		{
			camera->SetFov(glm::radians(fov));
		}

		if (Controls::Float("Near", &near, 0.1f))
		{
			camera->SetNear(near);
		}

		if (Controls::Float("Far", &far, 0.1f))
		{
			camera->SetFar(far);
		}
	}

	template<>
	void Component(Engine::Component::MeshRender* component)
	{
		ImGui::TextDisabled(component->mesh.ToString().c_str());
	}

	template<>
	void Component(Engine::Component::DirectionalLight* component)
	{
		ImGui::ColorEdit4("Color", &component->color.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
		ImGui::SliderFloat("Intensity", &component->intensity, 0.1f, 5.f);
	}
};