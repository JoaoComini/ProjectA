#pragma once

#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "Scene.hpp"

namespace Engine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(Engine::Scene& scene);

		void Serialize(std::filesystem::path path);
		void Deserialize(std::filesystem::path path);
	private:

		template<typename T>
		void Serialize(YAML::Emitter& emitter, Entity entity)
		{
			if (T* comp = entity.TryGetComponent<T>())
			{
				Serialize(emitter, comp);
			}
		}

		template<typename T>
		void Serialize(YAML::Emitter& emitter, T* comp);

		template<typename T>
		void Deserialize(YAML::Node& node, std::string key, Entity entity)
		{
			if (auto value = node[key])
			{
				T* comp = entity.TryGetComponent<T>();

				if (!comp)
				{
					comp = &entity.AddComponent<T>();
				}

				Deserialize(value, comp);
			}
		}

		template<typename T>
		void Deserialize(YAML::Node& node, T* comp);

		Engine::Scene& scene;
	};
};