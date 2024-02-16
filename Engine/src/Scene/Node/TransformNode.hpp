#pragma once

#include "Math/Transform.hpp"

#include "Node.hpp"

namespace Engine
{
	class TransformNode : public Node
	{
	REFLECT(TransformNode)

	public:
		TransformNode();
		TransformNode(const TransformNode&);

		void SetPosition(const glm::vec3& position);
		glm::vec3 GetPosition() const;

		void SetRotation(const glm::vec3& euler);
		glm::vec3 GetRotation() const;

		void SetScale(const glm::vec3& scale);
		glm::vec3 GetScale() const;

		void PropagateGlobalChanges();

		void SetTransform(const Transform& transform);
		Transform GetTransform();

		void SetGlobalTransform(const Transform& transform);
		Transform GetGlobalTransform();

		virtual void OnSceneAdd() override;
		virtual void OnSceneRemove() override;
		virtual void OnTransformChanged() {}

		virtual Node* Clone() const override;

	private:
		void UpdateLocalTransform();

		void SetDirtyBit(uint8_t bit);
		void ClearDirtyBit(uint8_t bit);
		bool IsDirtyBit(uint8_t bit) const;

	private:
		uint8_t dirty{ 0 };

		enum DirtyBits : uint8_t
		{
			NONE = 0,
			LOCAL = 1,
			GLOBAL = 2,
		};

		glm::vec3 euler{ 0 };
		glm::vec3 scale{ 1 };

		Transform local{};
		Transform global{};

		TransformNode* parent{};
		std::set<TransformNode*> children;
	};
}