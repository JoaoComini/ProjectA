#pragma once

#include "Rendering/Mesh.h"
#include "Rendering/Camera.h"

#include "Entity.h"

namespace glm
{
	template <class Archive>
	void Serialize(Archive& ar, quat& quat)
	{
		ar(quat.x, quat.y, quat.z, quat.w);
	}
}

namespace Engine::Component
{
	struct Transform
	{
		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
		glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

		[[nodiscard]] glm::mat4 GetLocalMatrix() const
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
		std::shared_ptr<Mesh> mesh{ nullptr };
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

	template <typename...>
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
	>;;

	template <class Archive>
	void Serialize(Archive& ar, Name& name)
	{
		ar(name.name);
	}

	template <class Archive>
	void Serialize(Archive& ar, Children& children)
	{
		ar(children.size);
		ar(children.first);
	}

	template <class Archive>
	void Serialize(Archive& ar, Hierarchy& hierarchy)
	{
		ar(hierarchy.parent);
		ar(hierarchy.next);
		ar(hierarchy.prev);
	}

	template <class Archive>
	void Serialize(Archive& ar, Transform& transform)
	{
		ar(transform.position);
		ar(transform.rotation);
		ar(transform.scale);
	}

	template <class Archive>
	void Serialize(Archive& ar, MeshRender& meshRender)
	{
		ar(meshRender.mesh);
	}

	template <class Archive>
	void Serialize(Archive& ar, Camera& camera)
	{
		ar(camera.camera);
	}

	template <class Archive>
	void Serialize(Archive& ar, DirectionalLight& light)
	{
		ar(light.color);
		ar(light.intensity);
	}

	template <class Archive>
	void Serialize(Archive& ar, PointLight& light)
	{
		ar(light.color);
		ar(light.range);
	}

	template <class Archive>
	void Serialize(Archive& ar, Script& script)
	{
		ar(script.script);
	}

	template <class Archive>
	void Serialize(Archive& ar, PhysicsBody& body)
	{
		ar(body.type);
	}

	template <class Archive>
	void Serialize(Archive& ar, BoxShape& shape)
	{
		ar(shape.size);
		ar(shape.offset);
	}

	template <class Archive>
	void Serialize(Archive& ar, SphereShape& shape)
	{
		ar(shape.radius);
		ar(shape.offset);
	}
}
