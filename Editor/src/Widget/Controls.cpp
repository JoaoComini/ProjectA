#include "Controls.h"

#include <imgui.h>

#include <Scene/Components.h>

#include <Rendering/Camera.h>

#include <Resource/ResourceManager.h>

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

			ImGui::TextUnformatted(label.c_str());

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
		auto& camera = component->camera;

		float fov = glm::degrees(camera.GetFov());
		float near = camera.GetNear();
		float far = camera.GetFar();

		if (Controls::Float("FOV", &fov))
		{
			camera.SetFov(glm::radians(fov));
		}

		if (Controls::Float("Near", &near, 0.1f))
		{
			camera.SetNear(near);
		}

		if (Controls::Float("Far", &far, 0.1f))
		{
			camera.SetFar(far);
		}
	}

	template<>
	void Component(Engine::Component::MeshRender* component)
	{
		ImGui::TextDisabled("%s", component->mesh->GetId().ToString().c_str());
	}

	template<>
	void Component(Engine::Component::DirectionalLight* component)
	{
		ImGui::ColorEdit4("Color", &component->color.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
		ImGui::SliderFloat("Intensity", &component->intensity, 0.1f, 1000.f);
	}

	template<>
	void Component(Engine::Component::PointLight* component)
	{
		ImGui::ColorEdit4("Color", &component->color.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
		ImGui::SliderFloat("Range", &component->range, 0.1f, 1000.f);
	}

	template<>
	void Component(Engine::Component::SkyLight* component)
	{
		ImGui::TextDisabled("%s", component->cubemap.ToString().c_str());
	}

	template<>
	void Component(Engine::Component::Script* component)
	{
		auto currentId = component->script;
		auto currentMapping = Engine::ResourceRegistry::Get().FindMappingById(currentId);

		if (ImGui::BeginCombo("##ScriptCombo", currentMapping ? currentMapping->path.string().c_str() : ""))
		{
			auto entries = Engine::ResourceRegistry::Get().GetEntriesByType(Engine::ResourceType::Script);

			for (const auto& [id, metadata] : entries)
			{
				bool selected = currentId == id;

				if (ImGui::Selectable(metadata.path.string().c_str(), selected))
				{
					currentId = id;
					component->script = currentId;
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
	}

	template<>
	void Component(Engine::Component::PhysicsBody* component)
	{
		const std::array<std::string, 3> strings { "Static", "Kinematic", "Dynamic"};

		std::string current = strings[(int) component->type];

		if (ImGui::BeginCombo("Body Type", current.c_str()))
		{
			for (size_t i = 0; i < strings.size(); i++)
			{
				auto& type = strings[i];

				bool selected = i == (int) component->type;

				if (ImGui::Selectable(type.c_str(), selected))
				{
					component->type = static_cast<Engine::Component::PhysicsBody::MotionType>(i);
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
	}

	template<>
	void Component(Engine::Component::BoxShape* component)
	{
		Controls::Vec3("Size", component->size);
		Controls::Vec3("Offset", component->offset);
	}

	template<>
	void Component(Engine::Component::SphereShape* component)
	{
		Controls::Float("Radius", &component->radius);
		Controls::Vec3("Offset", component->offset);
	}
};