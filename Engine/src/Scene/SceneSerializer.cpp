#include "SceneSerializer.hpp"

#include "Scene.hpp"

#include <fstream>

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::quat>
	{
		static Node encode(const glm::quat& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::quat& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Engine::Uuid>
	{

		static bool decode(const Node& node, Engine::Uuid& rhs)
		{
			rhs = node.as<uint64_t>();
			return true;
		}
	};

	template <typename T>
	YAML::Emitter& operator<<(YAML::Emitter& emitter, const T& rhs) {
		emitter << convert<T>::encode(rhs);
		return emitter;
	}

	template <>
	YAML::Emitter& operator<<(YAML::Emitter& emitter, const Engine::Uuid& rhs) {
		emitter << static_cast<uint64_t>(rhs);
		return emitter;
	}
};

namespace Engine
{
	SceneSerializer::SceneSerializer(Engine::Scene& scene)
		: scene(scene)
	{
	}
	void SceneSerializer::Serialize(std::filesystem::path path)
	{
		YAML::Emitter emitter;

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Entities" << YAML::BeginSeq;

		scene.ForEachEntity([&](Entity entity) {
			emitter << YAML::BeginMap;

			Serialize<Component::Id>(emitter, entity);
			Serialize<Component::Name>(emitter, entity);
			Serialize<Component::Relationship>(emitter, entity);
			Serialize<Component::Transform>(emitter, entity);
			Serialize<Component::MeshRender>(emitter, entity);
			Serialize<Component::Camera>(emitter, entity);

			emitter << YAML::EndMap;
		});

		emitter << YAML::EndMap;
		emitter << YAML::EndSeq;

		std::ofstream file(path);
		file << emitter.c_str();
	}

	template<>
	void SceneSerializer::Serialize(YAML::Emitter& emitter, Component::Id* comp)
	{
		emitter << YAML::Key << "Id" << YAML::Value << comp->id;
	}

	template<>
	void SceneSerializer::Serialize(YAML::Emitter& emitter, Component::Name* comp)
	{
		emitter << YAML::Key << "Name" << YAML::Value << comp->name;
	}

	template<>
	void SceneSerializer::Serialize(YAML::Emitter& emitter, Component::Relationship* comp)
	{
		emitter << YAML::Key << "Relationship";
		emitter << YAML::BeginMap;

		emitter << YAML::Key << "Children" << YAML::Value << comp->children;

		if (comp->parent)
		{
			emitter << YAML::Key << "Parent" << YAML::Value << comp->parent.GetId();
		}

		if (comp->next)
		{
			emitter << YAML::Key << "Next" << YAML::Value << comp->next.GetId();
		}

		if (comp->prev)
		{
			emitter << YAML::Key << "Prev" << YAML::Value << comp->prev.GetId();
		}

		if (comp->first)
		{
			emitter << YAML::Key << "First" << YAML::Value << comp->first.GetId();
		}

		emitter << YAML::EndMap;
	}

	template<>
	void SceneSerializer::Serialize(YAML::Emitter& emitter, Component::Transform* comp)
	{
		emitter << YAML::Key << "Transform";

		emitter << YAML::BeginMap;

		emitter << YAML::Key << "Position" << YAML::Value << comp->position;
		emitter << YAML::Key << "Rotation" << YAML::Value << comp->rotation;
		emitter << YAML::Key << "Scale" << YAML::Value << comp->scale;

		emitter << YAML::EndMap;
	}

	template<>
	void SceneSerializer::Serialize(YAML::Emitter& emitter, Component::MeshRender* comp)
	{
		emitter << YAML::Key << "Mesh" << YAML::Value << comp->mesh;
	}

	template<>
	void SceneSerializer::Serialize(YAML::Emitter& emitter, Component::Camera* comp)
	{
		emitter << YAML::Key << "Camera";

		emitter << YAML::BeginMap;

		emitter << YAML::Key << "Far" << YAML::Value << comp->camera.GetFar();
		emitter << YAML::Key << "Near" << YAML::Value << comp->camera.GetNear();
		emitter << YAML::Key << "FOV" << YAML::Value << comp->camera.GetFov();

		emitter << YAML::EndMap;
	}

	void SceneSerializer::Deserialize(std::filesystem::path path)
	{
		YAML::Node scene = YAML::LoadFile(path.string());

		auto entities = scene["Entities"];

		if (!entities)
		{
			return;
		}

		for (auto entity : entities)
		{
			Uuid id{};
			if (auto value = entity["Id"])
			{
				id = value.as<Uuid>();
			}

			auto deserialized = this->scene.CreateEntity(id);

			Deserialize<Component::Name>(entity, "Name", deserialized);
			Deserialize<Component::Transform>(entity, "Transform", deserialized);
			Deserialize<Component::MeshRender>(entity, "Mesh", deserialized);
			Deserialize<Component::Camera>(entity, "Camera", deserialized);
		}

		for (auto entity : entities)
		{
			Uuid id{};
			if (auto value = entity["Id"])
			{
				id = value.as<Uuid>();
			}

			auto deserialized = this->scene.FindEntityById(id);

			Deserialize<Component::Relationship>(entity, "Relationship", deserialized);
		}
	}

	template<>
	void SceneSerializer::Deserialize(YAML::Node& node, Component::Name* comp)
	{
		comp->name = node.as<std::string>();
	}

	template<>
	void SceneSerializer::Deserialize(YAML::Node& node, Component::Transform* comp)
	{
		comp->position = node["Position"].as<glm::vec3>();
		comp->rotation = node["Rotation"].as<glm::quat>();
		comp->scale = node["Scale"].as<glm::vec3>();
	}

	template<>
	void SceneSerializer::Deserialize(YAML::Node& node, Component::MeshRender* comp)
	{
		comp->mesh = node.as<Uuid>();
	}

	template<>
	void SceneSerializer::Deserialize(YAML::Node& node, Component::Camera* comp)
	{
		comp->camera.SetFar(node["Far"].as<float>());
		comp->camera.SetNear(node["Near"].as<float>());
		comp->camera.SetFov(node["FOV"].as<float>());
	}

	template<>
	void SceneSerializer::Deserialize(YAML::Node& node, Component::Relationship* comp)
	{
		comp->children = node["Children"].as<size_t>();

		if (auto parent = node["Parent"])
		{
			comp->parent = scene.FindEntityById(parent.as<Uuid>());
		}

		if (auto next = node["Next"])
		{
			comp->next = scene.FindEntityById(next.as<Uuid>());
		}

		if (auto prev = node["Prev"])
		{
			comp->prev = scene.FindEntityById(prev.as<Uuid>());
		}

		if (auto first = node["First"])
		{
			comp->first = scene.FindEntityById(first.as<Uuid>());
		}
	}
};