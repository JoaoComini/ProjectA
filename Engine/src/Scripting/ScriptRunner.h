#pragma once

#include "ScriptInstance.h"

#include "Scene/SceneGraph.h"

#include <sol/sol.hpp>

namespace Engine
{
	class ScriptRunner
	{
	public:
		ScriptRunner(SceneGraph& scene);

		void Start();
		void Update(float delta);
		void Stop();

		ScriptInstance* FindScriptInstance(Entity::Id entity);

		SceneGraph& GetScene() const;
		sol::state_view GetState() const;

	private:
		SceneGraph& scene;
		sol::state lua;

		std::unordered_map<Entity::Id, ScriptInstance> instances;
	};
}