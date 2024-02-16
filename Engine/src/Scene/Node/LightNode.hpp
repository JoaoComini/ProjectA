#pragma once

#include "TransformNode.hpp"

#include "Common/Uuid.hpp"

#include <glm/glm.hpp>

namespace Engine
{
	class LightNode : public TransformNode
	{
	
	REFLECT(LightNode)

	public:
		LightNode();
		LightNode(const LightNode& node);

		void SetIntensity(float intensity);
		void SetColor(const glm::vec3& color);

		float GetIntensity() const;
		glm::vec3 GetColor() const;

	private:
		float intensity = 1.f;
		glm::vec3 color{ 1.f };

	protected:
		Uuid instance{ 0 };
	};

	class DirectionalLightNode : public LightNode
	{
		REFLECT(DirectionalLightNode)

	public:
		void OnSceneAdd() override;
		void OnSceneRemove() override;
		void OnTransformChanged() override;
	};

	class PointLightNode : public LightNode
	{
		REFLECT(PointLightNode)

	public:
		void OnSceneAdd() override;
		void OnSceneRemove() override;
		void OnTransformChanged() override;

		void SetRange(float range);
		float GetRange() const;
		
	private:
		float range = 1.f;
	};
}