#pragma once

#include <imgui.h>

#include <string>

namespace Controls
{
	struct ButtonCollors
	{
		ImVec4 normal;
		ImVec4 hovered;
		ImVec4 active;
	};

	void Vec3(const std::string& label, glm::vec3& values, float resetValue = 0.f);

	bool Float(const std::string& label, float* value, float min = 0.f, float max = 0.f);

	void ResetButton(const std::string& label, float* value, ButtonCollors colors = ButtonCollors{}, float resetValue = 0.f);

	template<typename T>
	void Component(T* component);
};