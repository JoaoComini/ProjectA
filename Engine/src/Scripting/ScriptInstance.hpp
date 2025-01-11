#pragma once

#include "Scene/Entity.hpp"

#include <sol/sol.hpp>

namespace Engine
{
	class Script;

	class ScriptInstance
	{
	public:
		ScriptInstance(sol::state& lua, Script& script, Entity::Id entity);

		void Start();
		void Update(float delta);
		void OnContactEnter(Entity::Id other);
		void OnContactExit(Entity::Id other);

		sol::environment& GetEnv();

	private:
		sol::environment env;

		sol::function startFn;
		sol::function updateFn;
		sol::function onContactEnterFn;
		sol::function onContactExitFn;

		Entity::Id entity;
	};
}