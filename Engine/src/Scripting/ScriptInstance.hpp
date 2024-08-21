#pragma once

#include "Scene/Entity.hpp"

#include <sol/sol.hpp>

namespace Engine
{
	class Script;

	class ScriptInstance
	{
	public:
		ScriptInstance(sol::state& lua, Script& script, Entity entity);

		void Start();
		void Update(float delta);
		void OnContactEnter(Entity other);
		void OnContactExit(Entity other);

		sol::environment& GetEnv();

	private:
		sol::environment env;

		sol::function startFn;
		sol::function updateFn;
		sol::function onContactEnterFn;
		sol::function onContactExitFn;

		Entity entity;
	};
}