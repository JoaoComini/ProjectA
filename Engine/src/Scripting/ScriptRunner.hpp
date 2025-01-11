#pragma once

#include "ScriptInstance.hpp"

#include "Scene/Scene.hpp"

#include <sol/sol.hpp>

namespace Engine
{
	class ScriptRunner
	{
	public:
		ScriptRunner(Scene& scene);

		void Start();
		void Update(float delta);
		void Stop();

		ScriptInstance* FindScriptInstance(Entity::Id entity);

		Scene& GetScene() const;
		sol::state_view GetState() const;

	private:
		Scene& scene;
		sol::state lua;

		std::unordered_map<Entity::Id, ScriptInstance> instances;
	};
}