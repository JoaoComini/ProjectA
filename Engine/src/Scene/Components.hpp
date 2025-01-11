#pragma once

#include "Rendering/Mesh.hpp"
#include "Rendering/Camera.hpp"

#include "Resource/Resource.hpp"

#include "Entity.hpp"

namespace Engine::Component
{
	struct Transform
	{
		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
		glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

		glm::mat4 GetLocalMatrix() const
		{
			return glm::translate(glm::mat4(1.0f), position)
				* glm::mat4_cast(rotation)
				* glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct LocalToWorld
	{
		glm::mat4 value{ 1 };
	};

	struct Name
	{
		std::string name = "Empty Entity";
	};

	struct Children
	{
		std::size_t size{ 0 };
		Entity::Id first{ Entity::Null };
	};

	struct Hierarchy
	{
		Entity::Id prev{ Entity::Null };
		Entity::Id next{ Entity::Null };
		Entity::Id parent{ Entity::Null };
	};

	struct MeshRender
	{
		ResourceId mesh{ 0 };
	};

	struct Camera
	{
		Engine::Camera camera;
	};

	struct DirectionalLight
	{
		glm::vec3 color{ 1.f };
		float intensity = 1.f;
	};

	struct PointLight
	{
		glm::vec3 color{ 1.f };
		float range = 1.f;
	};

	struct SkyLight
	{
		ResourceId cubemap{ 0 };
	};

	struct Script
	{
		ResourceId script{ 0 };
	};

	struct PhysicsBody
	{
		enum class MotionType : uint8_t { Static = 0, Kinematic,  Dynamic };

		MotionType type{ MotionType::Static };
		uint32_t id{ std::numeric_limits<uint32_t>::max() };

		glm::vec3 velocity{};
		glm::vec3 force{};
	};

	struct PhysicsContactEnter
	{
		Entity::Id other{ Entity::Null };
	};

	struct PhysicsContactExit
	{
		Entity::Id other{ Entity::Null };
	};

	struct BoxShape
	{
		glm::vec3 size{ 1, 1, 1 };
		glm::vec3 offset{ 0 };
	};

	struct SphereShape
	{
		float radius{ 1 };
		glm::vec3 offset{ 0 };
	};

	struct Delete {};

	template <typename... Component>
	struct Group { };

	using Serializable = Group<
		Transform,
		Name,
		Children,
		Hierarchy,
		MeshRender,
		Camera,
		DirectionalLight,
		PointLight,
		SkyLight,
		Script,
		PhysicsBody,
		BoxShape,
		SphereShape
	>;

};

