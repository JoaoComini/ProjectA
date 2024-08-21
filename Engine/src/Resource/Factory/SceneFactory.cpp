#include "SceneFactory.hpp"

#include <cereal/archives/json.hpp>

namespace glm
{
	template <class Archive>
	void Serialize(Archive& ar, vec3& vec)
	{
		ar(vec.x, vec.y, vec.z);
	}

	template <class Archive>
	void Serialize(Archive& ar, quat& quat)
	{
		ar(quat.x, quat.y, quat.z, quat.w);
	}
};

namespace Engine
{
	namespace Component
	{
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
		void Serialize(Archive& ar, SkyLight& light)
		{
			ar(light.cubemap);
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

    void SceneFactory::Create(std::filesystem::path destination, Scene& scene)
    {
        std::ofstream file(destination);

        cereal::JSONOutputArchive archive{ file };

        archive(scene);
    }

    std::shared_ptr<Scene> SceneFactory::Load(std::filesystem::path source)
    {
		std::ifstream file(source);

		cereal::JSONInputArchive archive{ file };

		auto scene = std::make_shared<Scene>();

		archive(*scene);

        return scene;
    }
};